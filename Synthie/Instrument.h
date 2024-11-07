#pragma once
#include "AudioNode.h"
#include "Note.h"
#define NUMEFFECTSCHANNELS 2
class CInstrument :
	public CAudioNode
{
public:
	CInstrument();
	CInstrument(double);
	virtual ~CInstrument();
	virtual void SetNote(CNote *note) = 0;
	virtual double Send(int i) { return sends[i]; }

private:
	double sends[NUMEFFECTSCHANNELS] = {1.0, 0.0}; // Default: dry only
};

