#include "stdafx.h"
#include "Synthesizer.h"
#include "Instrument.h"
#include "ToneInstrument.h"
#include "PianoInstrument.h"
#include "Reverb.h"
#include "Ring.h"
#include "Chorus.h"
#include "xmlhelp.h"
#include <vector>
#include <algorithm>

CSynthesizer::CSynthesizer()
	: m_time(0)
{
	CoInitialize(NULL);
	m_channels = 2;
	m_sampleRate = 44100.;
	m_samplePeriod = 1 / m_sampleRate;
	m_bpm = 120;
	m_beatspermeasure = 4;
	m_secperbeat = 0.5;
	m_reverb.SetSampleRate(GetSampleRate());
	m_ringModulation.SetSampleRate(GetSampleRate());
	m_chorus.SetSampleRate(GetSampleRate());
}


CSynthesizer::~CSynthesizer()
{
}

//! Start the synthesizer
void CSynthesizer::Start()
{
	/*m_time = 0;
	CToneInstrument *ti = new CToneInstrument();
	ti->SetSampleRate(GetSampleRate());
	ti->SetFreq(440);
	ti->SetDuration(3);
	ti->Start();
	m_instruments.push_back(ti);*/
	m_instruments.clear();
	m_currentNote = 0;
	m_measure = 0;
	m_beat = 0;
	m_time = 0;
	m_reverb.SetSampleRate(GetSampleRate());
}


bool CSynthesizer::Generate(double* frame)
{
	// Phase 1: Determine if any notes need to be played.
	while (m_currentNote < (int)m_notes.size())
	{
		CNote* note = &m_notes[m_currentNote];

		if (note->Measure() > m_measure)
			break;

		if (note->Measure() == m_measure && note->Beat() > m_beat)
			break;

		CInstrument* instrument = NULL;
		if (note->Instrument() == L"ToneInstrument")
		{
			instrument = new CToneInstrument(GetBeatsPerMinute());
		}
		else if (note->Instrument() == L"PianoInstrument")
		{
			instrument = new CPianoInstrument(GetBeatsPerMinute());
		}

		if (instrument != NULL)
		{
			instrument->SetSampleRate(GetSampleRate());
			instrument->SetNote(note);
			instrument->Start();

			m_instruments.push_back(instrument);
		}

		m_currentNote++;
	}

	// Phase 2: Clear all channels to silence
	for (int c = 0; c < GetNumChannels(); c++)
	{
		frame[c] = 0;
	}

	// Phase 3: Play active instruments
	double dryFrame[2] = {0}; // Accumulate dry signal
	double wetFrame[2] = {0}; // Accumulate wet signal (reverb)
	double ringFrame[2] = {0}; // Accumulate ring modulation signal
	double chorusFrame[2] = {0}; // Accumulate chorus signal

	for (list<CInstrument*>::iterator node = m_instruments.begin(); node != m_instruments.end();)
	{
		list<CInstrument*>::iterator next = node;
		next++;

		CInstrument* instrument = *node;

		if (instrument->Generate())
		{
			for (int c = 0; c < GetNumChannels(); ++c)
			{
				dryFrame[c] += instrument->Frame(c);
			}
		}
		else
		{
			m_instruments.erase(node);
			delete instrument;
		}

		node = next;
	}

	// Apply reverb as an effect instrument
	m_reverb.Process(dryFrame, wetFrame, GetNumChannels());

	// Apply ring modulation as an effect instrument
	m_ringModulation.Process(dryFrame, ringFrame, GetNumChannels());

	// Apply chorus as an effect instrument
	m_chorus.Process(dryFrame, chorusFrame, GetNumChannels());

	// Mix dry, wet, ring modulation, and chorus signals
	for (int c = 0; c < GetNumChannels(); ++c)
	{
		frame[c] = dryFrame[c] * 0.4 + wetFrame[c] * 0.2 + ringFrame[c] * 0.2 + chorusFrame[c] * 0.2; // Mix signals
	}

	// Phase 4: Advance the time and beats
	m_time += GetSamplePeriod();
	m_beat += GetSamplePeriod() / m_secperbeat;

	if (m_beat > m_beatspermeasure)
	{
		m_beat -= m_beatspermeasure;
		m_measure++;
	}

	// Phase 5: Determine when we are done
	return !m_instruments.empty() || m_currentNote < (int)m_notes.size();
}


// Get the time since we started generating audio
double CSynthesizer::GetTime()
{
	return m_time;
}
void CSynthesizer::Clear()
{
	m_instruments.clear();
	m_notes.clear();
}
void CSynthesizer::XmlLoadScore(IXMLDOMNode* xml)
{
	// Get a list of all attribute nodes and the
	// length of that list
	CComPtr<IXMLDOMNamedNodeMap> attributes;
	xml->get_attributes(&attributes);
	long len;
	attributes->get_length(&len);

	// Loop over the list of attributes
	for (int i = 0; i < len; i++)
	{
		// Get attribute i
		CComPtr<IXMLDOMNode> attrib;
		attributes->get_item(i, &attrib);

		// Get the name of the attribute
		CComBSTR name;
		attrib->get_nodeName(&name);

		// Get the value of the attribute.  A CComVariant is a variable
		// that can have any type. It loads the attribute value as a
		// string (UNICODE), but we can then change it to an integer 
		// (VT_I4) or double (VT_R8) using the ChangeType function 
		// and then read its integer or double value from a member variable.
		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == L"bpm")
		{
			value.ChangeType(VT_R8);
			m_bpm = value.dblVal;
			m_secperbeat = 1 / (m_bpm / 60);
		}
		else if (name == L"beatspermeasure")
		{
			value.ChangeType(VT_I4);
			m_beatspermeasure = value.intVal;
		}

	}


	CComPtr<IXMLDOMNode> node;
	xml->get_firstChild(&node);
	for (; node != NULL; NextNode(node))
	{
		// Get the name of the node
		CComBSTR name;
		node->get_nodeName(&name);
		if (name == L"instrument")
		{
			XmlLoadInstrument(node);
		}
	}
}
void CSynthesizer::OpenScore(CString& filename)
{
	Clear();

	//
	// Create an XML document
	//

	CComPtr<IXMLDOMDocument>  pXMLDoc;
	bool succeeded = SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
		IID_IXMLDOMDocument, (void**)&pXMLDoc));
	if (!succeeded)
	{
		AfxMessageBox(L"Failed to create an XML document to use");
		return;
	}

	// Open the XML document
	VARIANT_BOOL ok;
	succeeded = SUCCEEDED(pXMLDoc->load(CComVariant(filename), &ok));
	if (!succeeded || ok == VARIANT_FALSE)
	{
		AfxMessageBox(L"Failed to open XML score file");
		return;
	}

	//
	// Traverse the XML document in memory!!!!
	// Top level tag is <score>
	//

	CComPtr<IXMLDOMNode> node;
	pXMLDoc->get_firstChild(&node);
	for (; node != NULL; NextNode(node))
	{
		// Get the name of the node
		CComBSTR nodeName;
		node->get_nodeName(&nodeName);
		if (nodeName == "score")
		{
			XmlLoadScore(node);
		}
	}
	sort(m_notes.begin(), m_notes.end());
}
void CSynthesizer::XmlLoadInstrument(IXMLDOMNode* xml)
{
	wstring instrument = L"";

	// Get a list of all attribute nodes and the length of that list
	CComPtr<IXMLDOMNamedNodeMap> attributes;
	xml->get_attributes(&attributes);
	long len;
	attributes->get_length(&len);

	// Loop over the list of attributes
	for (int i = 0; i < len; i++)
	{
		CComPtr<IXMLDOMNode> attrib;
		attributes->get_item(i, &attrib);

		CComBSTR name;
		attrib->get_nodeName(&name);

		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == L"instrument")
		{
			instrument = value.bstrVal;
		}
	}

	CComPtr<IXMLDOMNode> node;
	xml->get_firstChild(&node);
	for (; node != NULL; NextNode(node))
	{
		CComBSTR name;
		node->get_nodeName(&name);

		if (name == L"note")
		{
			if (instrument == L"Reverb")
			{
				m_reverb.SetNoteAttributes(node);
			}
			else if (instrument == L"RingModulation")
			{
				m_ringModulation.SetNoteAttributes(node);
			}
			else if (instrument == L"Chorus")
			{
				m_chorus.SetNoteAttributes(node);
			}
			else
			{
				XmlLoadNote(node, instrument);
			}
		}
	}
}
void CSynthesizer::XmlLoadNote(IXMLDOMNode* xml, std::wstring& instrument)
{
	m_notes.push_back(CNote());
	CNote& note = m_notes.back();
	note.XmlLoad(xml, instrument);

	// Check for reverb attribute
	CComPtr<IXMLDOMNamedNodeMap> attributes;
	xml->get_attributes(&attributes);
	long len;
	attributes->get_length(&len);

	for (int i = 0; i < len; i++)
	{
		CComPtr<IXMLDOMNode> attrib;
		attributes->get_item(i, &attrib);

		CComBSTR name;
		attrib->get_nodeName(&name);

		CComVariant value;
		attrib->get_nodeValue(&value);

		if (name == L"reverb")
		{
			value.ChangeType(VT_BOOL);
			note.SetReverb(value.boolVal == VARIANT_TRUE);
		}
	}
}