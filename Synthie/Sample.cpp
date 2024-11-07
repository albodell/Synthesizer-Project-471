#include "stdafx.h"
#include "Sample.h"
#include <iostream>
#include <windows.h> // For PlaySound
#include <mmsystem.h> // For PlaySound
#pragma comment(lib, "winmm.lib") // Link with winmm.lib

Sample::Sample(const std::wstring & filepath) : m_filepath(filepath) {
    // Load the audio data from the file specified by filepath
    // This is a placeholder for actual file loading logic
    std::wcout << L"Loading audio data from: " << filepath << std::endl;
    // Populate m_audioData with actual audio data
}

std::vector<float> Sample::GetAudioData() const {
    return m_audioData;
}

void Sample::Play() const {
    // Use PlaySound to play the audio file
    if (!m_filepath.empty()) {
        std::wcout << L"Playing audio sample from file: " << m_filepath << std::endl;
        PlaySound(m_filepath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    } else {
        std::wcerr << L"File path is empty, cannot play audio." << std::endl;
    }
}