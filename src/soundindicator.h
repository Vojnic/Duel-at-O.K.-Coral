#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <random>

class SoundIndicator {
public:
    SoundIndicator(const std::string& fakeIndicatorPath, const std::string& realIndicatorPath);
    ~SoundIndicator();
    
    void playRandomIndicator(bool& inputDisabled);
	bool playRandomSoundWithDelay();
    bool wasLastIndicatorReal() const;
	bool isSoundFinished() const;
    void stop();

private:
    sf::SoundBuffer fakeBuffer;
    sf::SoundBuffer realBuffer;
    sf::Sound sound;
    bool lastPlayedReal;
	bool soundFinished;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
};
