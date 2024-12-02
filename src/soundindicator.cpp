#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <random>
#include <chrono>

class SoundIndicator {
public:
    SoundIndicator(const std::string& fakeIndicatorPath, const std::string& realIndicatorPath);
    ~SoundIndicator();

    void playRandomIndicator(bool& inputDisabled);
    bool playRandomSoundWithDelay();
    bool wasLastIndicatorReal() const;
    void stop();
    bool isSoundFinished() const;  // Check if the sound has finished

private:
    sf::SoundBuffer fakeBuffer;
    sf::SoundBuffer realBuffer;
    sf::Sound sound;
    bool lastPlayedReal;
    bool soundFinished;  // Track if the sound is finished
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
};

SoundIndicator::SoundIndicator(const std::string& fakeIndicatorPath, const std::string& realIndicatorPath)
    : lastPlayedReal(false),
    soundFinished(false),  // Initialize as not finished
    rng(std::chrono::steady_clock::now().time_since_epoch().count()),
    dist(0.0, 1.0) {

    if (!fakeBuffer.loadFromFile(fakeIndicatorPath)) {
        throw std::runtime_error("Failed to load fake indicator sound!");
    }
    if (!realBuffer.loadFromFile(realIndicatorPath)) {
        throw std::runtime_error("Failed to load real indicator sound!");
    }
}

SoundIndicator::~SoundIndicator() {
    sound.stop();
}

void SoundIndicator::playRandomIndicator(bool& inputDisabled) {
    if (sound.getStatus() == sf::Sound::Playing) {
        sound.stop(); // Stop the sound if it's playing before playing a new one
    }

    lastPlayedReal = dist(rng) < 0.5; // 50% chance

    if (lastPlayedReal) {
        sound.setBuffer(realBuffer);
    }
    else {
        sound.setBuffer(fakeBuffer);
    }

    sound.play();
    soundFinished = false;  // Reset before playing

    // Disable input until the real indicator plays
    if (lastPlayedReal) {
        inputDisabled = false; // Enable inputs when real sound plays
    }
}

bool SoundIndicator::wasLastIndicatorReal() const {
    return lastPlayedReal;
}

void SoundIndicator::stop() {
    sound.stop();
}

bool SoundIndicator::isSoundFinished() const {
    // Check if the sound is done playing
    return sound.getStatus() == sf::Sound::Stopped;
}

bool SoundIndicator::playRandomSoundWithDelay() {
    lastPlayedReal = dist(rng) < 0.5; 

    if (lastPlayedReal) {
        sound.setBuffer(realBuffer);
    }
    else {
        sound.setBuffer(fakeBuffer);
    }

    sound.play();


    return lastPlayedReal;
}
