#include <SFML/Graphics.hpp>

using namespace sf;

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    RenderWindow window(VideoMode::getDesktopMode(), "BomberMan", Style::Fullscreen,settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(30);


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(Color(100,100,100));

        window.display();
    }

    return 0;
}
