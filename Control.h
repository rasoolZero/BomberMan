#ifndef CONTROL_H
#define CONTROL_H
#include <SFML/Graphics.hpp>

using namespace sf;

class Control
{
    public:
        Control(RenderWindow & _window, int _width, int _height);

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

};

#endif // CONTROL_H
