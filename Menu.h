#ifndef MENU_H
#define MENU_H
#include <SFML/Graphics.hpp>
#include "Manager.h"
#include "Audio.h"
using namespace sf;

class Menu
{
public:
	Menu(RenderWindow& window, Color background_color, Texture* logo_texture, Manager& manager, Audio& audio);
	void update(Time DeltaTime);
	void manageKey(Event::KeyEvent key, bool released = false);
	void manageMouse(Event::MouseButtonEvent mouseButton, bool released = false);
private:
	RenderWindow& window;
	Manager& manager;
	Audio& audio;

	Vector2f middle;
	Color bg;
	Texture* logo_texture;
	Sprite logo;
	Text title[2];
};

#endif // !MENU_H

