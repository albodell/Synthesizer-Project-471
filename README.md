Ownership: Alex Bodell


This application is a comprehensive synthesizer that simulates a piano and supports various audio effects. It is designed to provide realistic piano sounds and includes features such as polyphony, envelopes, pedal simulation, and multiple audio effects.
Components
1. Piano Synthesis
Description: The piano synthesis component simulates the sound of an acoustic piano. It uses the Karplus-Strong string synthesis algorithm to generate realistic piano tones. This component supports all 88 keys of a standard piano.
Functionality:
Karplus-Strong Synthesis: Implements a feedback loop with a delay line to simulate string vibrations.
Frequency Control: Allows setting the frequency for each note, corresponding to the piano keys.
Grading Elements Supported:
Simulates the sound of an acoustic piano.
Demonstrates all 88 keys of a standard piano.
2. Polyphony
Description: Polyphony allows the synthesizer to play multiple notes simultaneously, similar to strumming a guitar or playing chords on a piano.
Functionality:
Multiple Note Handling: Supports starting and stopping multiple notes at the same time.
Concurrent Sound Generation: Manages multiple active notes to produce harmonious sounds.
Grading Elements Supported:
Ability to play more than one note at a time.
3. Envelopes
Description: Envelopes simulate the natural decay of a piano note when a key is released. This component ensures that notes do not abruptly stop but instead fade out naturally.
Functionality:
Attack, Decay, Sustain, Release (ADSR): Implements an envelope generator to control the amplitude of notes over time.
Dampening Simulation: Simulates the dampening effect when a key is released.
Grading Elements Supported:
Simulates the effect of key release with a dampening envelope.
4. Pedal Simulation
Description: The pedal simulation component mimics the effect of a sustain pedal on an acoustic piano, allowing notes to sustain for their full duration when the pedal is pressed.
Functionality:
Pedal Control: Integrates pedal actions into the score, allowing notes to sustain or dampen based on pedal state.
Sustain Logic: Overrides the dampening effect when the pedal is pressed.
Grading Elements Supported:
Simulates the sustain pedal effect in the score.
5. Effects
Description: The effects component enhances the audio output with various sound effects, including noise gating, chorus, reverberation, and ring modulation.
Functionality:
Chorus: Adds a shimmering effect by mixing delayed and modulated copies of the audio signal.
Reverberation: Simulates the natural echo of a room, adding depth to the sound.
Ring Modulation: Multiplies the audio signal with a modulating waveform to create metallic sounds.
Noise Gating: Reduces unwanted noise by cutting off low-level signals.
Grading Elements Supported:
Implements chorus, reverberation, and ring modulation effects.
