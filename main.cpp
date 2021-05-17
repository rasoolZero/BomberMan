#include <SFML/Graphics.hpp>
#include "Control.h"
#include <iostream>
//#define DEBUGGING

using namespace sf;

int control_height=200;

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    RenderWindow window(VideoMode::getDesktopMode(), "BomberMan", Style::Fullscreen,settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(30);
    Control controls(window,window.getSize().x,control_height);

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
