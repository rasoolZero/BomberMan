#ifndef AUDIO_H
#define AUDIO_H
#include "Resources_n.h"
#include <SFML/Audio.hpp>
#include <Thor/Resources.hpp>
#include <vector>

using namespace sf;

class Audio {
public:
  enum Sounds {
    Pause,
    Play,
    Rewind,
    Click,
    Failed,
    Capture,
    Intro,
    Boom,
    Winner,
    Music
  };
  Audio(thor::ResourceHolder<SoundBuffer, int> &soundBuffers,
        sf::Music *_music);
  void play(Sounds _sound);
  void stop(Sounds _sound);
  void setSound(bool _sound) { this->sound = _sound; }
  void setMasterVolume(float volume); // between 0 and 1
protected:
private:
  std::vector<Sound> sounds;
  thor::ResourceHolder<SoundBuffer, int> &buffers;
  sf::Music *music;
  bool sound = true;
  int const SOUNDS = Resources_n::soundsCount;
};

#endif // AUDIO_H
