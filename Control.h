#ifndef CONTROL_H
#define CONTROL_H
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "Audio.h"
#define BUTTONS 6

using namespace sf;

class Game;

class Control
{
    public:
        Control(RenderWindow & _window,Game & _game,Audio & _audio, int _width, int _height,thor::ResourceHolder<Texture,std::string> & _textures);
        void draw();
        void update();
        bool isPlaying(){return playing;}

    protected:

    private:
        thor::ResourceHolder<Texture,std::string> & textures;
        RectangleShape buttons[BUTTONS];
        RectangleShape background;
        RectangleShape soundButtons[2];

        RenderWindow &window;
        Audio & audio;
        Game & game;
        int width;
        int height;
        int speed=1;
        bool playing=true;
        bool sound=true;
        bool music=false;

};

#endif // CONTROL_H
