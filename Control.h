#ifndef CONTROL_H
#define CONTROL_H
#include <SFML/Graphics.hpp>
#include "Audio.h"
#define BUTTONS 6

using namespace sf;

class Control
{
    public:
        Control(RenderWindow & _window,Audio & _audio, int _width, int _height);
        void draw();
        void update(int *);
        int getFrameThreshold(){return frameThreshold;}
        bool isPlaying(){return playing;}

    protected:

    private:
        RectangleShape buttons[BUTTONS];
        RectangleShape background;
        Texture speedButtons[3];
        Texture rewindButton;
        Texture playButton;
        Texture pauseButton;
        Texture backwardButton;
        Texture forwardButton;

        Texture soundOn;
        Texture soundOff;
        Texture musicOn;
        Texture musicOff;
        RectangleShape soundButtons[2];


        RenderWindow &window;
        Audio &audio;
        int width;
        int height;
        int speed=1;
        int frameThreshold=15;
        bool playing=true;
        bool sound=true;
        bool music=false;

};

#endif // CONTROL_H
