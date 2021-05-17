#include <SFML/Graphics.hpp>
#include "Control.h"
#include <iostream>

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
        window.display();
    }

    return 0;
}
