#include <SFML/Graphics.hpp>
#include "Control.h"
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
    try{
        for(int i=0;i<3;i++)
            textures.acquire("speed_button_"+std::to_string(i+1),thor::Resources::fromFile<Texture>("assets/buttons/speed_button_"+std::to_string(i+1)+".png")).setSmooth(true);
        textures.acquire("rewind_button",thor::Resources::fromFile<Texture>("assets/buttons/rewind_button.png")).setSmooth(true);
        textures.acquire("play_button",thor::Resources::fromFile<Texture>("assets/buttons/play_button.png")).setSmooth(true);
        textures.acquire("pause_button",thor::Resources::fromFile<Texture>("assets/buttons/pause_button.png")).setSmooth(true);
        textures.acquire("forward_button",thor::Resources::fromFile<Texture>("assets/buttons/forward_button.png")).setSmooth(true);
        textures.acquire("backward_button",thor::Resources::fromFile<Texture>("assets/buttons/backward_button.png")).setSmooth(true);
        textures.acquire("sound_on",thor::Resources::fromFile<Texture>("assets/buttons/sound_on.png")).setSmooth(true);
        textures.acquire("sound_off",thor::Resources::fromFile<Texture>("assets/buttons/sound_off.png")).setSmooth(true);
        textures.acquire("music_on",thor::Resources::fromFile<Texture>("assets/buttons/music_on.png")).setSmooth(true);
        textures.acquire("music_off",thor::Resources::fromFile<Texture>("assets/buttons/music_off.png")).setSmooth(true);

        textures.acquire("floor",thor::Resources::fromFile<Texture>("assets/sprites/floor.png")).setSmooth(true);
        textures.acquire("box",thor::Resources::fromFile<Texture>("assets/sprites/box.png")).setSmooth(true);
        textures.acquire("obstacle",thor::Resources::fromFile<Texture>("assets/sprites/obstacle.png")).setSmooth(true);
        textures.acquire("powerup1",thor::Resources::fromFile<Texture>("assets/sprites/powerup1.png")).setSmooth(true);
        textures.acquire("powerup2",thor::Resources::fromFile<Texture>("assets/sprites/powerup2.png")).setSmooth(true);
        textures.acquire("fire",thor::Resources::fromFile<Texture>("assets/sprites/fire.png")).setSmooth(true);

        soundBuffers.acquire(0,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Pause.flac"));
        soundBuffers.acquire(1,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Play.flac"));
        soundBuffers.acquire(2,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Rewind.flac"));
        soundBuffers.acquire(3,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Click.flac"));
        soundBuffers.acquire(4,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Failed.flac"));
        soundBuffers.acquire(5,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Music.wav"));
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
    Game game(window,j,textures,CONTROL_WIDTH);
    Audio audio(soundBuffers);
    Control controls(window,game,audio,CONTROL_WIDTH,window.getSize().y,textures);

    #ifdef DEBUGGING
    Clock timer;
    timer.restart();

    Font font;
    font.loadFromFile("assets/fonts/Roboto-Light.ttf");
    Text text;
    text.setColor(Color::Red);
    text.setFont(font);
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

        window.clear(Color(100,100,100));
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
