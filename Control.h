#ifndef CONTROL_H
#define CONTROL_H
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "Manager.h"
#include "Audio.h"
#include "AdjustBar.h"
#define BUTTONS 6

using namespace sf;

class Game;

class Control
{
    public:
        Control(RenderWindow & _window, Manager& _manager,Game & _game, Audio & _audio, int _width,int _height, thor::ResourceHolder<Texture,std::string> & _textures);
        void load(int turnCount);
        void draw();
        void update(Time DeltaTime);
        void manageKey(Event::KeyEvent key, bool released = false);
        void manageMouse(Event::MouseButtonEvent mouseButton, bool released = false);
        void updateMouse(Event::MouseMoveEvent mouseMove);
        bool isPlaying(){return playing;}

    protected:

    private:
        thor::ResourceHolder<Texture,std::string> & textures;
        RectangleShape buttons[BUTTONS];
        RectangleShape background;
        RectangleShape soundButtons[2];
        AdjustBar turn_seek;
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
        bool adjusting = false;

        const Vector2f active_position = Vector2f(0, 0);
        const Vector2f inactive_position = Vector2f(0, -60);
        const Time activation_time = seconds(0.25);
        float progress = 0;
        Transformable button_transformable[BUTTONS];
        Transformable bar_transformable;
        IntRect active_area;

        void toggleSound(bool sound_selected);
        void setTurn(int turn);
        void setTurn(double progress);
        void setPlaying();
};

#endif // CONTROL_H
