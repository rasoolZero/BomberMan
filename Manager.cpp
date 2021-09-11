#include "Manager.h"
#include "Control.h"
#include "Game.h"
#include "Intro.h"
#include "Menu.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace sf;
Manager::Manager(RenderWindow *window_ptr, Color background_color,
                 Texture *logo_texture, Shader *shader)
    : window_ptr{window_ptr}, bg{background_color}, shader{shader},
      masks{{static_cast<Vector2f>(window_ptr->getSize()) / 2.f,
             {window_ptr->getSize().x +
                  static_cast<float>(window_ptr->getSize().y) * 125.f / 217.f,
              window_ptr->getSize().y / 2.f},
             seconds(2),
             {window_ptr->getSize().y * 125.f / 217.f,
              -static_cast<float>(window_ptr->getSize().y) / 2.f},
             Color(11, 0, 21)},
            {static_cast<Vector2f>(window_ptr->getSize()) / 2.f,
             {-static_cast<float>(window_ptr->getSize().y) * 125.f / 217.f,
              window_ptr->getSize().y / 2.f},
             seconds(2),
             {window_ptr->getSize().y * 125.f / 217.f,
              -static_cast<float>(window_ptr->getSize().y) / 2.f},
             Color(11, 0, 21)}} {
  next_screen.create(window_ptr->getSize().x, window_ptr->getSize().y);
  for (int i = 0; i < 2; i++) {
    masks[i].setBackTexture(&next_screen.getTexture());
    masks[i].setBodyTexture(logo_texture, logo_texture->getSize().y /
                                              (window_ptr->getSize().y * 1.4f));
    masks[i].setBodyTextureColor(Color(150, 150, 200));
    masks[i].setTailPowerScale(0.8);
    masks[i].setBodyShader(shader);
  }
}

void Manager::setPointers(
    Intro *intro_ptr, Menu *menu_ptr, Control *control_ptr, Game *game_ptr,
    Audio *audio, thor::ResourceHolder<SoundBuffer, int> *soundBuffers) {
  this->intro_ptr = intro_ptr;
  this->menu_ptr = menu_ptr;
  this->control_ptr = control_ptr;
  this->game_ptr = game_ptr;
  this->audio_ptr = audio;
  this->soundBuffers = soundBuffers;
}

void Manager::setState(State state) {
  if (this->active_screen == State::intro) {
    for (int i = 0; i < Resources_n::soundsCount; i++) {
      if (Resources_n::sounds[i] == "Intro") {
        soundBuffers->release(i);
        break;
      }
    }
  }
  if (active_screen != State::intro) {
    transiting = true;
    next_state = state;
  } else {
    active_screen = state;
  }
}

void Manager::update(Time DeltaTime) {
  switch (this->active_screen) {
  case State::game:
    game_ptr->update(DeltaTime);
    control_ptr->update(DeltaTime);
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
  if (transiting) {
    next_screen.clear(bg);
    switch (active_screen) {
    case Manager::game:
      menu_ptr->draw(&next_screen);
      break;
    case Manager::menu:
      game_ptr->draw(&next_screen);
      control_ptr->draw(&next_screen);
      break;
    default:
      break;
    }
    next_screen.display();
    masks[0].update(DeltaTime);
    masks[1].update(DeltaTime);
    if (active_screen == State::menu) {
      audio_ptr->setMasterVolume(masks[0].getProgress());
    } else if (active_screen == State::game) {
      audio_ptr->setMasterVolume(1 - masks[0].getProgress());
    }
    window_ptr->draw(masks[0]);
    window_ptr->draw(masks[1]);
    if (masks[0].getProgress() == 1.f && masks[1].getProgress() == 1.f) {
      transiting = false;
      masks[0].reset();
      masks[1].reset();
      if (active_screen == State::game) {
        unloadGame();
      }
      this->active_screen = next_state;
    }
  }
}

void Manager::manageInput(Event event) {
  if (transiting) {
    return;
  }
  if (event.type == Event::KeyPressed || event.type == Event::KeyReleased) {
    bool released = false;
    if (event.type == Event::KeyReleased) {
      released = true;
    }
    switch (active_screen) {
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
  } else if (event.type == Event::MouseMoved) {
    event.mouseMove.x = Mouse::getPosition(*window_ptr).x;
    event.mouseMove.y = Mouse::getPosition(*window_ptr).y;
    switch (active_screen) {
    case Manager::intro:
      break;
    case Manager::menu:
      menu_ptr->updateMouse(event.mouseMove);
      break;
    case Manager::game:
      control_ptr->updateMouse(event.mouseMove);
      break;
    default:
      break;
    }
  } else if (event.type == Event::MouseButtonPressed ||
             event.type == Event::MouseButtonReleased) {
    bool released = false;
    if (event.type == Event::MouseButtonReleased) {
      released = true;
    }
    event.mouseButton.x = Mouse::getPosition(*window_ptr).x;
    event.mouseButton.y = Mouse::getPosition(*window_ptr).y;
    switch (active_screen) {
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

void Manager::loadMenu(double progress) { menu_ptr->appear(progress); }

void Manager::loadGame(std::wstring log_dir) {
  game_ptr->load(log_dir);
  control_ptr->load(game_ptr->getLastTurn());
}

void Manager::unloadGame() { control_ptr->unload(); }
