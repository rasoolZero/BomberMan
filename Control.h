#ifndef CONTROL_H
#define CONTROL_H
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "Manager.h"
#include "Audio.h"
#define BUTTONS 6

using namespace sf;

class Game;

class Control
{
    public:
        Control(RenderWindow & _window, Manager& _manager,Game & _game, Audio & _audio, int _width,int _height, thor::ResourceHolder<Texture,std::string> & _textures);
        void draw();
        void update();
        void manageKey(Event::KeyEvent key, bool released = false);
        void manageMouse(Event::MouseButtonEvent mouseButton, bool released = false);
        bool isPlaying(){return playing;}

    protected:

    private:
        thor::ResourceHolder<Texture,std::string> & textures;
        RectangleShape buttons[BUTTONS];
        RectangleShape background;
        RectangleShape soundButtons[2];

        RenderWindow &window;
        Manager& manager;
        Audio & audio;
        Game & game;
        int width;
        int height;
        int speed=1;
        bool playing=false;
        bool sound=true;
        bool music=false;

        void toggleSound(bool sound_selected);
};

#endif // CONTROL_H
