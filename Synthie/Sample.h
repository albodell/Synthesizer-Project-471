// Sample.h
#pragma once
#include <string>
#include <vector>

class Sample {
public:
    Sample(const std::wstring& filepath);
    std::vector<float> GetAudioData() const;
    void Play() const; // Declare the Play method
    // Other methods and members
private:
    std::wstring m_filepath; // Store the file path for PlaySound
    std::vector<float> m_audioData; // Example member to store audio data
};

