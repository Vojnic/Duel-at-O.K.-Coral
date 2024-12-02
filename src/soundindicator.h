#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <random>

class SoundIndicator {
public:
    SoundIndicator(const std::string& fakeIndicatorPath, const std::string& realIndicatorPath);
    ~SoundIndicator();

    bool playRandomIndicator(bool& inputDisabled);
    bool wasLastIndicatorReal() const;
    void setLastIndicatorReal(bool real);
    bool isSoundFinished() const;
    void stop();

private:
    sf::SoundBuffer fakeBuffer;
    sf::SoundBuffer realBuffer;
    sf::Sound sound;
    bool lastPlayedReal;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
};
