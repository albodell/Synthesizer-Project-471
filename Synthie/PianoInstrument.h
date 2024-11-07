#pragma once
#include "Instrument.h"
#include <vector>
#include <queue>
#include "AR.h"


class CPianoInstrument : public CInstrument
{
public:
    CPianoInstrument();
    CPianoInstrument(double bpm);
    const double NUM_SECS_IN_MINUTE = 60.0;

    virtual ~CPianoInstrument();
    virtual void Start();
    virtual bool Generate();
    void SetNoteFrequency(double frequency);
    virtual void SetNote(CNote* note);

private:

    CAR m_ar;

    std::queue<double> m_delayLine;
    double m_sampleRate;
    double m_frequency;
    double m_decayFactor;
    int m_delayLineLength;
    double m_bpm;
};

