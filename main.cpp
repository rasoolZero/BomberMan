#include <SFML/Graphics.hpp>
#include "Control.h"
#include "Audio.h"
#include <iostream>
#include <Thor/Resources.hpp>
//#define DEBUGGING
#define CONTROL_WIDTH 150
using namespace sf;


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

        soundBuffers.acquire(0,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Pause.wav"));
        soundBuffers.acquire(1,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Play.wav"));
        soundBuffers.acquire(2,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Rewind.wav"));
        soundBuffers.acquire(3,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Click.wav"));
        soundBuffers.acquire(4,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Failed.wav"));
        soundBuffers.acquire(5,thor::Resources::fromFile<SoundBuffer>("assets/sounds/Music.wav"));
    }
    catch(thor::ResourceLoadingException& e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    RenderWindow window(VideoMode::getDesktopMode(), "BomberMan", Style::Fullscreen,settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(30);
    Audio audio(soundBuffers);
    Control controls(window,audio,CONTROL_WIDTH,window.getSize().y,textures);

    int stateCounter=0;
    int frameCounter=0;

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
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
                if(event.mouseButton.button == sf::Mouse::Left)
                    controls.update(&stateCounter);
            if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
        }

        if(controls.isPlaying()){
            frameCounter++;
            if(frameCounter>=controls.getFrameThreshold()){
                frameCounter=0;
                stateCounter++;
            }
        }

        window.clear(Color(100,100,100));
        controls.draw();
        #ifdef DEBUGGING
        Time t = timer.restart();
        text.setString("state counter: "+std::to_string(stateCounter)+"\nFPS: "+std::to_string(1/t.asSeconds()));
        window.draw(text);
        #endif // DEBUGGING
        window.display();
    }

    return 0;
}
