#include <SFML/Graphics.hpp>
#include "Manager.h"
#include "Intro.h"
#include "Menu.h"
#include "Control.h"
#include "Resources_n.h"
#include "Audio.h"
#include "Game.h"
#include <iostream>
#include <dirent.h>
#include <Thor/Resources.hpp>
#include <json.hpp>
#include "tinyfiledialogs.h"
#include "macros.h"
#define OS Windows
#define CONTROL_WIDTH 150
using namespace sf;
using json = nlohmann::json;

void capture(sf::RenderWindow & window){
    Texture capture;
    capture.update(window);
    Image img = capture.copyToImage();//window.capture();

    size_t maxFound = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("Screenshots\\")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            size_t number=0;
            std::string s(ent->d_name);
            size_t t = s.find_first_of("0123456789");
            if(t!=std::string::npos){
                number = std::stoi(ent->d_name+t);
                if(number>maxFound)
                    maxFound=number;
            }
        }
        closedir(dir);
    }
    else {
        perror ("Screenshots folder can't be opened");
        exit(EXIT_FAILURE);
    }

    img.saveToFile(std::string("Screenshots\\Scr") + std::to_string(maxFound+1) + std::string(".png") );
}

int main()
{
    Time DeltaTime;
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
        std::string message = e.what();
        while (message.find_first_of('\"') != std::string::npos) {
            message[message.find_first_of('\"')] = '*';
        }
        while (message.find_first_of('\'') != std::string::npos) {
            message[message.find_first_of('\'')] = '*';
        }
        tinyfd_messageBox("missing file!", message.c_str(), "ok", "error", 1);
        std::cout << e.what() << std::endl;
        return 1;
    }
    #if !defined OS
    #error OS is not defined.
    #endif
    #if(OS == Windows)
        system("mkdir Screenshots 2> nul:");
    #else
        system("mkdir Screenshots 2> /dev/null");
    #endif
    Clock clk;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    RenderWindow window(VideoMode::getDesktopMode(), "BomberMan", Style::Fullscreen,settings);

    Color bg(10, 10, 10);
    Manager manager(&window);
    window.setVerticalSyncEnabled(true);
    //game.load("log.json");
    Audio audio(soundBuffers);
    Game game(window,audio,textures,fonts,CONTROL_WIDTH);
    Intro intro(window, bg, &textures["logo"], manager, audio);
    Menu menu(window, bg, fonts[0], &textures["logo"], manager, audio);
    Control controls(window, manager,game,audio,CONTROL_WIDTH,window.getSize().y,textures);
    manager.setPointers(&intro, &menu, &controls, &game);

    #ifdef DEBUGGING
    Clock timer;
    timer.restart();

    Text text;
    text.setFillColor(Color::Red);
    text.setFont(fonts[0]);
    text.setCharacterSize(20);
    #endif // DEBUGGING
    while (window.isOpen())
    {
        DeltaTime = clk.restart();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2) {
                capture(window);
                audio.play(Audio::Capture);
            }
            else if (event.type == Event::MouseMoved || event.type == sf::Event::MouseButtonPressed || event.type == Event::MouseButtonReleased
            || event.type == Event::KeyPressed || event.type == Event::KeyReleased) {
                manager.manageInput(event);
            }
        }

        window.clear(bg);
        //controls.draw();
        //game.update();
        manager.update(DeltaTime);
        #ifdef DEBUGGING
        //if (manager.getState() == Manager::State::game) {
            Time t = timer.restart();
            text.setString("step: " + std::to_string(game.getTurn()) + "\nFPS: " + std::to_string((int)(1 / t.asSeconds())));
            window.draw(text);
        //}
        #endif // DEBUGGING
        window.display();
    }

    return 0;
}
