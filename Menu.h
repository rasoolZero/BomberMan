#ifndef MENU_H
#define MENU_H
#include <SFML/Graphics.hpp>
#include "Audio.h"
using namespace sf;

class Menu
{
public:
	Menu(RenderWindow& window, Color background_color, Texture* logo_texture, Audio& audio);
	void update(Time DeltaTime);
private:
	RenderWindow& window;
	Audio& audio;

	Vector2f middle;
	Color bg;
	Texture* logo_texture;
	Sprite logo;
	Text title[2];
};

#endif // !MENU_H

