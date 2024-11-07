#include "stdafx.h"

#include "Reverb.h"
#include <cmath>

CReverb::CReverb() : m_sampleRate(44100.0), m_feedback(0.7), m_wetLevel(0.5), m_dryLevel(0.5), m_delayTime(1.0)
{
    InitializeDelayLines();
}

CReverb::~CReverb()
{
}

void CReverb::SetSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
    InitializeDelayLines();
}

void CReverb::InitializeDelayLines()
{
    m_delayLengths = { 441, 353, 647, 907 };
    m_delayLines.resize(m_delayLengths.size());
    m_delayIndices.resize(m_delayLengths.size(), 0);

    for (size_t i = 0; i < m_delayLengths.size(); ++i)
    {
        m_delayLines[i].resize(m_delayLengths[i], 0.0);
    }
}

void CReverb::SetNoteAttributes(IXMLDOMNode* xml)
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
    }
}

void CReverb::Process(double* inputFrame, double* outputFrame, int numChannels)
{
    for (int c = 0; c < numChannels; ++c)
    {
        double inputSample = inputFrame[c];
        double outputSample = 0.0;

        for (size_t i = 0; i < m_delayLines.size(); ++i)
        {
            int& index = m_delayIndices[i];
            std::vector<double>& delayLine = m_delayLines[i];

            double delayedSample = delayLine[index];
            double newSample = inputSample + delayedSample * m_feedback;

            delayLine[index] = newSample;
            outputSample += delayedSample;

            index = (index + 1) % m_delayLengths[i];
        }

        outputFrame[c] = inputSample * m_dryLevel + outputSample * m_wetLevel;
    }
}
