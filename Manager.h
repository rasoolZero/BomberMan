#ifndef MANAGER_H
#define MANAGER_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;
class Intro;
class Menu;
class Control;
class Game;

class Manager
{
public:
	enum State {
		intro, menu, game
	};
	Manager(RenderWindow* window_ptr);
	void setPointers(Intro* intro_ptr, Menu* menu_ptr, Control* control_ptr, Game* game_ptr);
	void setState(State state);
	void update();
private:
	State active_screen;
	Intro* intro_ptr;
	Menu* menu_ptr;
	Control* control_ptr;
	Game* game_ptr;
	RenderWindow* window_ptr;
};

#endif // !MANAGER_H


