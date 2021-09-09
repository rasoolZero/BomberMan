#ifndef MANAGER_H
#define MANAGER_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include <SFML/Audio.hpp>
#include "Audio.h"
#include "Resources_n.h"
#include "Mask.h"
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
	Manager(RenderWindow* window_ptr, Color background_color, Texture* logo_texture, Shader* shader);
	void setPointers(Intro* intro_ptr, Menu* menu_ptr, Control* control_ptr, Game* game_ptr, Audio* audio, thor::ResourceHolder<SoundBuffer,int>* soundBuffers);
	void setState(State state = State::intro);
	State getState() { return this->active_screen; }
	void update(Time DeltaTime);
	void manageInput(Event event);
	void loadMenu(double progress);
	void loadGame(std::wstring log_dir);
	void unloadGame();
private:
	State active_screen = State::intro;
	State next_state;
	RenderTexture next_screen;
	Intro* intro_ptr;
	Menu* menu_ptr;
	Control* control_ptr;
	Game* game_ptr;
	Audio* audio_ptr;
    thor::ResourceHolder<SoundBuffer,int>* soundBuffers;
	RenderWindow* window_ptr;
	Color bg;

	bool transiting;
	Mask masks[2];
	RenderTexture new_screen;
	Texture* logo_texture;
	Shader* shader;
};

#endif // !MANAGER_H


