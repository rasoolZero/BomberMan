#include "Audio.h"
using namespace sf;

Audio::Audio()
{
    for(int i=0;i<SOUNDS;i++){
        buffers.push_back(SoundBuffer());
    }
    buffers[0].loadFromFile("assets/sounds/Pause.wav");
    buffers[1].loadFromFile("assets/sounds/Play.wav");
    buffers[2].loadFromFile("assets/sounds/Rewind.wav");
    buffers[3].loadFromFile("assets/sounds/Click.wav");
    buffers[4].loadFromFile("assets/sounds/Failed.wav");
    buffers[5].loadFromFile("assets/sounds/Music.wav");
    for(int i=0;i<buffers.size();i++){
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
