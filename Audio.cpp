#include "Audio.h"
using namespace sf;

Audio::Audio( thor::ResourceHolder<SoundBuffer,int> &soundBuffers) : buffers(soundBuffers)
{
    for(int i=0;i<SOUNDS;i++){
        sounds.push_back(Sound(buffers[i]));
    }
    sounds[SOUNDS-1].setLoop(true);
    sounds[SOUNDS-1].setVolume(80.0);
}

void Audio::play(Sounds _sound){
    int i=static_cast<int>(_sound);
    if(i==(SOUNDS-1))
        sounds[i].play();
    else
        if(this->sound)
            sounds[i].play();
}
void Audio::stop(Sounds _sound){
    sounds[static_cast<int>(_sound)].stop();
}
