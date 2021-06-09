#ifndef AUDIO_H
#define AUDIO_H
#include <vector>
#include <SFML/Audio.hpp>
#define SOUNDS 6

using namespace sf;

class Audio
{
    public:
        enum Sounds{Pause,Play,Rewind,Click,Failed,Music};
        Audio();
        void play(Sounds _sound);
        void stop(Sounds _sound);
        void setSound(bool _sound){this->sound=_sound;}

    protected:

    private:
    std::vector<Sound> sounds;
    std::vector<SoundBuffer> buffers;
    bool sound=true;
};

#endif // AUDIO_H
