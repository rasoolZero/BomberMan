#ifndef CONTROL_H
#define CONTROL_H
#include <SFML/Graphics.hpp>

using namespace sf;

class Control
{
    public:
        Control(RenderWindow & _window, int _width, int _height);
        void draw();
        void update(int *);
        int getFrameThreshold(){return frameThreshold;}
        bool isPlaying(){return playing;}

    protected:

    private:
        RectangleShape buttons[4];
        RectangleShape background;
        Texture speedButtons[3];
        Texture rewindButton;
        Texture playButton;
        Texture pauseButton;
        RenderWindow &window;
        int width;
        int height;
        int speed=1;
        int frameThreshold=15;
        bool playing=true;

};

#endif // CONTROL_H
