#include "stdafx.h"
#include "PianoInstrument.h"
#include <cstdlib>
#include <ctime>
#include "Notes.h"

CPianoInstrument::CPianoInstrument()
{
    m_sampleRate = 44100.0; // Example sample rate
    m_decayFactor = 0.996;  // Decay factor for the feedback loop
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed for random number generation
    m_bpm = 120.0; // Default BPM
}

CPianoInstrument::CPianoInstrument(double bpm)
{
    m_sampleRate = 44100.0; // Example sample rate
    m_decayFactor = 0.996;  // Decay factor for the feedback loop
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed for random number generation
    m_bpm = bpm; // Set BPM
}

CPianoInstrument::~CPianoInstrument()
{
}

void CPianoInstrument::Start()
{

    m_delayLine = std::queue<double>();
    m_delayLineLength = static_cast<int>(m_sampleRate / m_frequency);

    // Initialize the delay line with random noise
    for (int i = 0; i < m_delayLineLength; ++i)
    {
        m_delayLine.push(static_cast<double>(std::rand()) / RAND_MAX - 0.5);
    }
}

bool CPianoInstrument::Generate()
{
    if (m_delayLine.empty())
        return false;

    // Get the first sample from the delay line
    double firstSample = m_delayLine.front();
    m_delayLine.pop();

    // Get the next sample from the delay line
    double nextSample = m_delayLine.front();

    // Apply the simple low-pass filter
    double newSample = m_decayFactor * 0.5 * (firstSample + nextSample);

    // Push the new sample back into the delay line
    m_delayLine.push(newSample);

    // Output the current sample
    m_frame[0] = newSample;
    m_frame[1] = newSample;

    return true;
}

void CPianoInstrument::SetNoteFrequency(double frequency)
{
    m_frequency = frequency;
    // Calculate delay line length based on frequency and sample rate
    m_delayLineLength = static_cast<int>(m_sampleRate / m_frequency);
    // Initialize delay line with zeros
    while (!m_delayLine.empty()) m_delayLine.pop();
    for (int i = 0; i < m_delayLineLength; ++i)
    {
        m_delayLine.push(0.0);
    }
}

void CPianoInstrument::SetNote(CNote* note)
{
    // Get a list of all attribute nodes and the length of that list
    CComPtr<IXMLDOMNamedNodeMap> attributes;
    note->Node()->get_attributes(&attributes);
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

        // Get the value of the attribute
        CComVariant value;
        attrib->get_nodeValue(&value);

        if (name == "duration")
        {
            value.ChangeType(VT_R8);
            // Set the duration for the note
            // Assuming m_ar is an envelope generator or similar
            m_ar.SetDuration(value.dblVal * (NUM_SECS_IN_MINUTE / m_bpm));
        }
        else if (name == "note")
        {
            SetNoteFrequency(NoteToFrequency(value.bstrVal));
        }
    }
}

