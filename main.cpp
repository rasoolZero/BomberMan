#include <SFML/Graphics.hpp>
#include "Control.h"
#include "Resources_n.h"
#include "Audio.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <Thor/Resources.hpp>
#include <json.hpp>
#define DEBUGGING
#define CONTROL_WIDTH 150
using namespace sf;
using json = nlohmann::json;
Time DeltaTime;

int main()
{
    thor::ResourceHolder<Texture,std::string> textures;
    thor::ResourceHolder<SoundBuffer,int> soundBuffers;
    thor::ResourceHolder<Font,int> fonts;
    try{
        for(int i=0;i<Resources_n::texturesCount;i++)
            textures.acquire(Resources_n::textures[i],thor::Resources::fromFile<Texture>("assets/sprites/"+Resources_n::textures[i]+".png")).setSmooth(true);
        textures["player1"].setSmooth(false);
        textures["player2"].setSmooth(false);
        for(int i=0;i<Resources_n::spritesCount;i++)
            textures.acquire(Resources_n::sprites[i],thor::Resources::fromFile<Texture>("assets/sprites/"+Resources_n::sprites[i]+".png")).setSmooth(true);

        for(int i=0;i<Resources_n::soundsCount;i++)
            soundBuffers.acquire(i,thor::Resources::fromFile<SoundBuffer>("assets/sounds/"+Resources_n::sounds[i]+".flac"));
        fonts.acquire(0,thor::Resources::fromFile<Font>("assets/fonts/Roboto-Light.ttf"));
    }
    catch(thor::ResourceLoadingException& e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    std::ifstream i("log.json");
    json j;
    i >> j;
    i.close();
    Clock clk;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    RenderWindow window(VideoMode::getDesktopMode(), "BomberMan", Style::Fullscreen,settings);
//    window.setVerticalSyncEnabled(true);
    Game game(window,j,textures,fonts,CONTROL_WIDTH);
    Audio audio(soundBuffers);
    Control controls(window,game,audio,CONTROL_WIDTH,window.getSize().y,textures);

    #ifdef DEBUGGING
    Clock timer;
    timer.restart();

    Text text;
    text.setColor(Color::Red);
    text.setFont(fonts[0]);
    text.setCharacterSize(20);
    #endif // DEBUGGING

    while (window.isOpen())
    {
        DeltaTime = clk.restart();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
                if(event.mouseButton.button == sf::Mouse::Left)
                    controls.update();
            if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
        }

        window.clear();
        controls.draw();
        game.update();
        #ifdef DEBUGGING
        Time t = timer.restart();
        text.setString("state: "+std::to_string(game.getTurn())+"\nFPS: "+std::to_string((int)(1/t.asSeconds())));
        window.draw(text);
        #endif // DEBUGGING
        window.display();
    }

    return 0;
}
