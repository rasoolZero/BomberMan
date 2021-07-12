#ifndef AUDIO_H
#define AUDIO_H
#include <vector>
#include <SFML/Audio.hpp>
#include <Thor/Resources.hpp>
#include <Resources_n.h>

using namespace sf;

class Audio
{
    public:
        enum Sounds{Pause,Play,Rewind,Click,Failed,Capture,Music};
        Audio( thor::ResourceHolder<SoundBuffer,int> & soundBuffers);
        void play(Sounds _sound);
        void stop(Sounds _sound);
        void setSound(bool _sound){this->sound=_sound;}

    protected:

    private:
    std::vector<Sound> sounds;
    thor::ResourceHolder<SoundBuffer,int> & buffers;
    bool sound=true;
    int const SOUNDS = Resources_n::soundsCount;
};

#endif // AUDIO_H
