#include "stdafx.h"
#include "Ring.h"
#include <cmath>

// Define M_PI if it's not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CRingModulation::CRingModulation() : m_sampleRate(44100.0), m_modulationFrequency(440.0), m_wetLevel(0.5), m_dryLevel(0.5), m_phase(0.0)
{
    m_phaseIncrement = 2.0 * M_PI * m_modulationFrequency / m_sampleRate;
}

CRingModulation::~CRingModulation()
{
}

void CRingModulation::SetSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
    m_phaseIncrement = 2.0 * M_PI * m_modulationFrequency / m_sampleRate;
}

void CRingModulation::SetNoteAttributes(IXMLDOMNode* xml)
{
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

        if (name == L"modulationFrequency")
        {
            value.ChangeType(VT_R8);
            m_modulationFrequency = value.dblVal;
            m_phaseIncrement = 2.0 * M_PI * m_modulationFrequency / m_sampleRate;
        }
        else if (name == L"wet")
        {
            value.ChangeType(VT_R8);
            m_wetLevel = value.dblVal;
        }
        else if (name == L"dry")
        {
            value.ChangeType(VT_R8);
            m_dryLevel = value.dblVal;
        }
    }
}

void CRingModulation::Process(double* inputFrame, double* outputFrame, int numChannels)
{
    for (int c = 0; c < numChannels; ++c)
    {
        double inputSample = inputFrame[c];
        double modulator = sin(m_phase);
        double ringModulatedSample = inputSample * modulator;

        outputFrame[c] = inputSample * m_dryLevel + ringModulatedSample * m_wetLevel;

        m_phase += m_phaseIncrement;
        if (m_phase >= 2.0 * M_PI)
        {
            m_phase -= 2.0 * M_PI;
        }
    }
}
