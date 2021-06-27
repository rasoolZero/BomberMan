#ifndef GAME_H
#define GAME_H
#include <json.hpp>
#include <Thor/Resources.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;
using json = nlohmann::json;
extern Time DeltaTime;

class Game
{
    public:
        Game(RenderWindow & _window,json & _json,thor::ResourceHolder<Texture,std::string> & _textures , unsigned offset);
        void update();
    protected:

    private:
        RenderWindow &window;
        json & json_;
        thor::ResourceHolder<Texture,std::string> & textures;
        int rows;
        int columns;
        int totalTurns;
        float scale;
        Vector2f startPoint;
        int initialHealth;
        VertexArray vertices;


        void draw();
};

#endif // GAME_H
