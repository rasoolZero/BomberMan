#ifndef GAME_H
#define GAME_H
#include <json.hpp>
#include <Thor/Resources.hpp>
#include <Thor/Animations.hpp>
#include <SFML/Graphics.hpp>
#include <Player_Action.h>
using namespace sf;
using json = nlohmann::json;
extern Time DeltaTime;

class Control;

class Game
{
    public:
        Game(RenderWindow & _window,json & _json,thor::ResourceHolder<Texture,std::string> & _textures , unsigned offset);
        void update();
        void changeSpeed(int _speed);

        bool setTurn(unsigned _turn);
        unsigned getTurn(){return turn;}

        void setPlaying(bool _playing);
        bool getPlaying(){return playing;}

    protected:

    private:
        RenderWindow &window;
        json & json_;
        thor::ResourceHolder<Texture,std::string> & textures;
        RectangleShape obstacle,box,deadzone;
        int rows;
        int columns;
        int totalTurns;
        float scale;
        Vector2f startPoint;
        int initialHealth;
        VertexArray vertices;

        unsigned turn;
        float timePassed=0;
        float timeThreshold=0.5;
        bool playing=true;
        int speed=1;

        RectangleShape powerup1,powerup2,fire,bomb,mine;
        thor::FrameAnimation powerupAnimation,fireAnimation,bombAnimation,mineAnimation;
        thor::Animator<sf::RectangleShape, std::string> powerupAnimator,fireAnimator,bombAnimator,mineAnimator;

        float timeThresholds[4]={0,0.6,0.3,0.2};

        void draw();
};

#endif // GAME_H
