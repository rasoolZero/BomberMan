#include "Manager.h"
#include "Intro.h"
#include "Control.h"
#include "Game.h"
#include "Menu.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;
Manager::Manager(RenderWindow* window_ptr)
	:window_ptr{ window_ptr }
{
}

void Manager::setPointers(Intro* intro_ptr, Menu* menu_ptr, Control* control_ptr, Game* game_ptr) {
	this->intro_ptr = intro_ptr;
	this->menu_ptr = menu_ptr;
	this->control_ptr = control_ptr;
	this->game_ptr = game_ptr;
}

void Manager::setState(State state) {
	this->active_screen = state;
}

void Manager::update(Time DeltaTime) {
	switch (this->active_screen) {
	case State::game:
		game_ptr->update(DeltaTime);
		control_ptr->draw();
		break;
	case State::menu:
		menu_ptr->update(DeltaTime);
		break;
	case State::intro:
		intro_ptr->update(DeltaTime);
		break;
	default:
		break;
	}
}

void Manager::manageInput(Event event)
{
	if (event.type == Event::KeyPressed || event.type == Event::KeyReleased) {
		bool released = false;
		if (event.type == Event::KeyReleased) {
			released = true;
		}
		switch (active_screen)
		{
		case State::intro:
			intro_ptr->manageKey(event.key, released);
			break;
		case State::menu:
			menu_ptr->manageKey(event.key, released);
			break;
		case State::game:
			control_ptr->manageKey(event.key, released);
			break;
		default:
			break;
		}
	}
	else if (event.type == Event::MouseMoved) {
		switch (active_screen)
		{
		case Manager::intro:
			break;
		case Manager::menu:
			menu_ptr->updateMouse(event.mouseMove);
			break;
		case Manager::game:
			break;
		default:
			break;
		}
	}
	else if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased) {
		bool released = false;
		if (event.type == Event::MouseButtonReleased) {
			released = true;
		}
		switch (active_screen)
		{
		case State::intro:
			intro_ptr->manageMouse(event.mouseButton, released);
			break;
		case State::menu:
			menu_ptr->manageMouse(event.mouseButton, released);
			break;
		case State::game:
			control_ptr->manageMouse(event.mouseButton, released);
			break;
		default:
			break;
		}
	}
}

void Manager::loadGame(std::string log_dir)
{
	game_ptr->load(log_dir);
}
