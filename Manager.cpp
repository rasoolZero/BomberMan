#include "Manager.h"
#include "Intro.h"
#include "Control.h"
#include "Game.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;
Manager::Manager(RenderWindow* window_ptr)
	:window_ptr{ window_ptr }
{
}

void Manager::setPointers(Intro* intro_ptr, /*Menu* menu_ptr,*/ Control* control_ptr, Game* game_ptr) {
	this->intro_ptr = intro_ptr;
	this->menu_ptr = menu_ptr;
	this->control_ptr = control_ptr;
	this->game_ptr = game_ptr;
}

void Manager::setState(State state) {
	this->active_screen = state;
}

void Manager::update() {
	switch (this->active_screen) {
	case State::game:
		control_ptr->draw();
		game_ptr->update();
		break;
	case State::menu:
		//menu_ptr->update;
		break;
	case State::intro:
		intro_ptr->update();
		break;
	default:
		break;
	}
}
