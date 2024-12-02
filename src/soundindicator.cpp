#include "SoundIndicator.h"
#include <stdexcept>
#include <chrono>

SoundIndicator::SoundIndicator(const std::string& fakeIndicatorPath, const std::string& realIndicatorPath)
    : lastPlayedReal(false),
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

bool SoundIndicator::playRandomIndicator(bool& inputDisabled) {
    if (sound.getStatus() == sf::Sound::Playing) {
        sound.stop();
    }
    lastPlayedReal = dist(rng) < 0.5;
    sound.setBuffer(lastPlayedReal ? realBuffer : fakeBuffer);
    sound.play();

    inputDisabled = !lastPlayedReal;

    return lastPlayedReal;
}

bool SoundIndicator::wasLastIndicatorReal() const {
    return lastPlayedReal;
}

void SoundIndicator::setLastIndicatorReal(bool real) {
    lastPlayedReal = real;
}

void SoundIndicator::stop() {
    sound.stop();
}

bool SoundIndicator::isSoundFinished() const {
    return sound.getStatus() == sf::Sound::Stopped;
}
