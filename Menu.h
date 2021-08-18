#ifndef MENU_H
#define MENU_H
#include <SFML/Graphics.hpp>
#include "Manager.h"
#include "Audio.h"
#define BUTTONCOUNT 2
#define INFOCOUNT 4
using namespace sf;

class Menu
{
public:
	Menu(RenderWindow& window, Color background_color, Font& font, Texture* logo_texture, Manager& manager, Audio& audio);
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
	enum texts {
		AICup, Bomberman, file, error
	};
	Text info[INFOCOUNT];
	enum button {
		choose, play
	};
	Text buttons[BUTTONCOUNT];
	const char* fileTypes[1] = { "*.json" };
	std::string log_dir;
	const Time message_display_time = seconds(2);
	Time message_timer = Time::Zero;

	void chooseFile();
};

#endif // !MENU_H

