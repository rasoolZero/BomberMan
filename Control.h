#ifndef CONTROL_H
#define CONTROL_H
#include <SFML/Graphics.hpp>
#define BUTTONS 6

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
        RectangleShape buttons[BUTTONS];
        RectangleShape background;
        Texture speedButtons[3];
        Texture rewindButton;
        Texture playButton;
        Texture pauseButton;
        Texture backwardButton;
        Texture forwardButton;
        RenderWindow &window;
        int width;
        int height;
        int speed=1;
        int frameThreshold=15;
        bool playing=true;

};

#endif // CONTROL_H
