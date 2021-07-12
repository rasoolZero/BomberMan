#include "Audio.h"
using namespace sf;

Audio::Audio( thor::ResourceHolder<SoundBuffer,int> &soundBuffers) : buffers(soundBuffers)
{
    for(int i=0;i<SOUNDS;i++){
        sounds.push_back(Sound());
        sounds[i].setBuffer(buffers[i]);
    }
    sounds[static_cast<int>(Sounds::Music)].setLoop(true);
    sounds[static_cast<int>(Sounds::Music)].setVolume(45.0);
    sounds[static_cast<int>(Sounds::Capture)].setVolume(60.0);
}

void Audio::play(Sounds _sound){
    int i=static_cast<int>(_sound);
    if(i==(SOUNDS-1))
        sounds[i].play();
    else
        if(this->sound){
            sounds[i].play();
        }
}
void Audio::stop(Sounds _sound){
    sounds[static_cast<int>(_sound)].stop();
}
