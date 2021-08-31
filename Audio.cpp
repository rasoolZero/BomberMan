#include "Audio.h"
using namespace sf;

Audio::Audio( thor::ResourceHolder<SoundBuffer,int> &soundBuffers, sf::Music* _music)
    : buffers(soundBuffers)
    ,music(_music)
{
    for(int i=0;i<SOUNDS;i++){
        sounds.push_back(Sound());
        sounds[i].setBuffer(buffers[i]);
    }
    music->setLoop(true);
    music->setVolume(45.0);
    sounds[static_cast<int>(Sounds::Capture)].setVolume(60.0);
    sounds[static_cast<int>(Sounds::Intro)].setPlayingOffset(seconds(1.1f));
    sounds[static_cast<int>(Sounds::Intro)].setVolume(70.f);
    //sounds[static_cast<int>(Sounds::Intro)].setPitch(1.02f);
    sounds[static_cast<int>(Sounds::Boom)].setVolume(60.f);
}

void Audio::play(Sounds _sound){
    int i=static_cast<int>(_sound);
    if (i == (static_cast<int>(Sounds::Music))) {
        music->play();
    }
    else if(this->sound){
        sounds[i].play();
    }
}
void Audio::stop(Sounds _sound){
    if (_sound == (static_cast<int>(Sounds::Music))) {
        music->stop();
    }
    else {
        sounds[static_cast<int>(_sound)].stop();
    }
}
