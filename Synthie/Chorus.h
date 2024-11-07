#pragma once
#include <vector>
#include <string>
#include <comdef.h> // For CComPtr and CComBSTR

class CChorus
{
public:
    CChorus();
    ~CChorus();

    void SetSampleRate(double sampleRate);
    void Process(double* inputFrame, double* outputFrame, int numChannels);
    void SetNoteAttributes(IXMLDOMNode* xml);

private:
    double m_sampleRate;
    double m_wetLevel;
    double m_dryLevel;
    double m_delayTime;
    double m_depth;
    double m_rate;

    std::vector<double> m_delayLine;
    int m_delayIndex;
    double m_lfoPhase;
    double m_lfoIncrement;
};

