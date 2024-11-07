#pragma once
#include <vector>
#include <string>
#include <comdef.h> // For CComPtr and CComBSTR

class CRingModulation
{
public:
    CRingModulation();
    ~CRingModulation();

    void SetSampleRate(double sampleRate);
    void Process(double* inputFrame, double* outputFrame, int numChannels);
    void SetNoteAttributes(IXMLDOMNode* xml);

private:
    double m_sampleRate;
    double m_modulationFrequency;
    double m_wetLevel;
    double m_dryLevel;

    double m_phase;
    double m_phaseIncrement;
};

