#ifndef GAME_H
#define GAME_H
#include <json.hpp>
#include <Thor/Resources.hpp>
#include <Thor/Animations.hpp>
#include <SFML/Graphics.hpp>
#include "Player_Action.h"
#include <unordered_map>
#define ANIMATOR thor::Animator<sf::RectangleShape, std::string>
using namespace sf;
using json = nlohmann::json;

class Control;

class Game
{

    public:
        Game(RenderWindow & _window,thor::ResourceHolder<Texture,std::string> & _textures ,
            thor::ResourceHolder<Font,int> & _fonts, unsigned _offset);
        void update(Time DeltaTime);
        void changeSpeed(int _speed);
        void load(std::string logAddress);

        bool setTurn(int _turn);
        bool setTurn(double progress); // progress between 0 and 1
        int getTurn(){return this->turn;}

        void setPlaying(bool _playing);
        bool getPlaying(){return playing;}

    protected:

    private:
        const sf::Color player1Theme = sf::Color(156,40,40);
        const sf::Color player2Theme = sf::Color(40,40,156);
        RenderWindow &window;
        json json_;
        thor::ResourceHolder<Texture,std::string> & textures;
        RectangleShape obstacle,box,deadzone,bombMask,winnerDisplay,player[2], heart[2], playerInfo;
        int rows;
        int columns;
        int totalTurns;
        float scale;
        Vector2f startPoint;
        int initialHealth;
        int bombDelay;
        VertexArray vertices;
        sf::RenderTexture rTexture;

        sf::BlendMode blendMode=sf::BlendAdd;

        int const playerInfoBoxHeight = 50;
        int health[2];
        int heartTextureSize;
        int const fontSize=17;
        Text upgrades[2], names[2], extraHealth[2], winnerText;
        unsigned offset;

        int turn;
        float timePassed=0;
        float timeThreshold;
        bool playing=false;
        int speed=1;

        std::unordered_map<std::string,RectangleShape> shapes;
        std::unordered_map<std::string,thor::FrameAnimation> animations;
        std::unordered_map<std::string,ANIMATOR> animators;


        float timeThresholds[4]={0,0.6,0.3,0.2};

        void draw();
        void displayWinner();
        Vector2f getPlayerPosition(int _turn,int player);
        void updatePlayer();
        void initPlayerAnimation();
};

#endif // GAME_H
