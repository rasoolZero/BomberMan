#ifndef GAME_H
#define GAME_H
#include <json.hpp>
#include <Thor/Resources.hpp>
#include <Thor/Animations.hpp>
#include <SFML/Graphics.hpp>
#include "Player_Action.h"
#include "Audio.h"
#include <unordered_map>
#define ANIMATOR thor::Animator<sf::RectangleShape, std::string>
using namespace sf;
using json = nlohmann::json;

class Control;

class Game
{

    public:
        Game(RenderWindow & _window, Audio & _audio, thor::ResourceHolder<Texture,std::string> & _textures ,
            thor::ResourceHolder<Font,int> & _fonts, unsigned _offset);
        void update(Time DeltaTime);
        void updateMouse(Vector2f position);
        void changeSpeed(int _speed);
        float getTurnTime() { return this->timeThreshold; }
        void load(std::string logAddress);

        bool setTurn(int _turn);
        bool setTurn(double progress); // progress between 0 and 1
        int getTurn(){return this->turn;}
        int getLastTurn() { return this->totalTurns; }

        void setPlaying(bool _playing);
        bool getPlaying(){return playing;}

        void showFullName(bool player, bool show);

    protected:

    private:
        const sf::Color player1Theme = sf::Color(255, 176, 32);
        const sf::Color player2Theme = sf::Color(0, 200, 255);
        RenderWindow &window;
        Audio &audio;
        json json_;
        thor::ResourceHolder<Texture,std::string> & textures;
        RectangleShape obstacle,box,deadzone,bombMask,winnerDisplay,player[2], heart[2], playerInfo;
        int rows;
        int columns;
        int totalTurns;
        float scale;
        Vector2f startPoint;
        Vector2f infoSize;
        const int playerInfoLeftOffset = 5;
        int initialHealth;
        int bombDelay;
        VertexArray vertices;
        sf::RenderTexture rTexture;

        sf::BlendMode blendMode=sf::BlendAdd;

        unsigned controlHeight;
        int health[2];
        int heartTextureSize;
        int const fontSize=18;
        int const minFontSize = 12;
        Text upgrades[2], names[2], fullnames[2], extraHealth[2], winnerText;
        float three_dot_width;
        bool truncated[2];
        bool showing_fullname[2];
        Transformable player_info_transformable[2];

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
        void setupTurn();
};

#endif // GAME_H
