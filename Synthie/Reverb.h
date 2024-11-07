#pragma once
#include <vector>
#include <string>
#include <comdef.h> // For CComPtr and CComBSTR

class CReverb
{
public:
    CReverb();
    ~CReverb();

    void SetSampleRate(double sampleRate);
    void Process(double* inputFrame, double* outputFrame, int numChannels);
    void SetNoteAttributes(IXMLDOMNode* xml);

private:
    double m_sampleRate;
    std::vector<std::vector<double>> m_delayLines;
    std::vector<int> m_delayIndices;
    std::vector<int> m_delayLengths;
    double m_feedback;
    double m_wetLevel;
    double m_dryLevel;
    double m_delayTime;

    void InitializeDelayLines();
};
