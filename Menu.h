#ifndef MENU_H
#define MENU_H
#include "Audio.h"
#include "Manager.h"
#include <SFML/Graphics.hpp>

#define BUTTONCOUNT 2
#define INFOCOUNT 4
using namespace sf;

class Menu {
public:
  Menu(RenderWindow &window, Color background_color, Font &font,
       Texture *logo_texture, Manager &manager, Audio &audio);
  void update(Time DeltaTime);
  void draw(RenderTarget *target = nullptr);
  void appear(double progress);
  void manageKey(Event::KeyEvent key, bool released = false);
  void manageMouse(Event::MouseButtonEvent mouseButton, bool released = false);
  void updateMouse(Event::MouseMoveEvent mouseMove);

private:
  RenderWindow &window;
  Manager &manager;
  Audio &audio;

  Vector2f middle;
  Color bg;
  Texture *logo_texture;
  Sprite logo;
  bool exiting_flag = false;
  enum Texts { AICup, Bomberman, file, error };
  Text info[INFOCOUNT];
  enum Button { choose, play };
  Text buttons[BUTTONCOUNT];
  Button selected = Button::choose;
  IntRect selectboxes[BUTTONCOUNT];
  const Color selected_button_color = Color(30, 180, 250);
  const Color normal_button_color = Color(250, 250, 250);
  const Color color_difference = normal_button_color - selected_button_color;
  const Time button_color_change_time = seconds(0.25);

  const char *fileTypes[1] = {"*.json"};
  std::wstring log_dir;

  const Time message_display_time = seconds(2);
  Time message_timer = Time::Zero;

  void chooseFile();
  void load();
};

#endif // !MENU_H
