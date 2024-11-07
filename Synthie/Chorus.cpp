#include "stdafx.h"
#include "Chorus.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
CChorus::CChorus() : m_sampleRate(44100.0), m_wetLevel(0.5), m_dryLevel(0.5), m_delayTime(0.03), m_depth(0.002), m_rate(0.25), m_delayIndex(0), m_lfoPhase(0.0)
{
    m_lfoIncrement = 2.0 * M_PI * m_rate / m_sampleRate;
    int maxDelaySamples = static_cast<int>((m_delayTime + m_depth) * m_sampleRate);
    m_delayLine.resize(maxDelaySamples, 0.0);
}

CChorus::~CChorus()
{
}

void CChorus::SetSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
    m_lfoIncrement = 2.0 * M_PI * m_rate / m_sampleRate;
    int maxDelaySamples = static_cast<int>((m_delayTime + m_depth) * m_sampleRate);
    m_delayLine.resize(maxDelaySamples, 0.0);
}

void CChorus::SetNoteAttributes(IXMLDOMNode* xml)
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

        if (name == L"wet")
        {
            value.ChangeType(VT_R8);
            m_wetLevel = value.dblVal;
        }
        else if (name == L"dry")
        {
            value.ChangeType(VT_R8);
            m_dryLevel = value.dblVal;
        }
        else if (name == L"delay")
        {
            value.ChangeType(VT_R8);
            m_delayTime = value.dblVal;
        }
        else if (name == L"depth")
        {
            value.ChangeType(VT_R8);
            m_depth = value.dblVal;
        }
        else if (name == L"rate")
        {
            value.ChangeType(VT_R8);
            m_rate = value.dblVal;
            m_lfoIncrement = 2.0 * M_PI * m_rate / m_sampleRate;
        }
    }
}

void CChorus::Process(double* inputFrame, double* outputFrame, int numChannels)
{
    for (int c = 0; c < numChannels; ++c)
    {
        double inputSample = inputFrame[c];

        // Calculate the current delay in samples
        double currentDelay = m_delayTime + m_depth * sin(m_lfoPhase);
        int delaySamples = static_cast<int>(currentDelay * m_sampleRate);

        // Get the delayed sample
        int readIndex = (m_delayIndex - delaySamples + m_delayLine.size()) % m_delayLine.size();
        double delayedSample = m_delayLine[readIndex];

        // Store the current sample in the delay line
        m_delayLine[m_delayIndex] = inputSample;

        // Increment the delay line index and wrap around if necessary
        m_delayIndex = (m_delayIndex + 1) % m_delayLine.size();

        // Mix the dry and wet signals
        outputFrame[c] = inputSample * m_dryLevel + delayedSample * m_wetLevel;

        // Increment the LFO phase and wrap around if necessary
        m_lfoPhase += m_lfoIncrement;
        if (m_lfoPhase >= 2.0 * M_PI)
        {
            m_lfoPhase -= 2.0 * M_PI;
        }
    }
}
