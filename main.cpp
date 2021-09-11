#include "Audio.h"
#include "Control.h"
#include "Game.h"
#include "Intro.h"
#include "Manager.h"
#include "Menu.h"
#include "Resources_n.h"
#include "macros.h"
#include "tinyfiledialogs.h"
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#define APPIMAGE 0

#define CONTROL_HEIGHT 20
using namespace sf;
using json = nlohmann::json;
namespace fs = std::filesystem;

bool capture(sf::RenderWindow &window) {
  Texture texture;
  if (!texture.create(window.getSize().x, window.getSize().y))
    return false;

  texture.update(window);
  Image img = texture.copyToImage(); // window.capture();

  size_t maxFound = 0;
  for (const auto &entry : fs::directory_iterator("Screenshots")) {
    size_t number = 0;
    std::filesystem::path pathOfEntry = entry.path();
    std::wstring s = pathOfEntry.filename().wstring();
    size_t t = s.find_first_of(L"0123456789");
    if (t != std::wstring::npos) {
      number = std::stoi(s.c_str() + t);
      if (number > maxFound)
        maxFound = number;
    }
  }
  img.saveToFile(std::string("Screenshots/Scr") + std::to_string(maxFound + 1) +
                 std::string(".png"));
  return true;
}

int main() {
  Time DeltaTime;
  thor::ResourceHolder<Texture, std::string> textures;
  thor::ResourceHolder<Shader, std::string> shaders;
  thor::ResourceHolder<SoundBuffer, int> soundBuffers;
  thor::ResourceHolder<Font, int> fonts;
  Music music;
  auto appDir = static_cast<std::string>("./assets/");

#if APPIMAGE
  appDir = static_cast<std::string>(getenv("APPDIR")) +
           "/usr/share/bomberman/assets/";
#endif

  try {
    for (int i = 0; i < Resources_n::texturesCount; i++)
      textures
          .acquire(Resources_n::textures[i],
                   thor::Resources::fromFile<Texture>(
                       appDir + "sprites/" + Resources_n::textures[i] + ".png"))
          .setSmooth(true);
    textures["player1"].setSmooth(false);
    textures["player2"].setSmooth(false);
    for (int i = 0; i < Resources_n::spritesCount; i++)
      textures
          .acquire(Resources_n::sprites[i],
                   thor::Resources::fromFile<Texture>(
                       appDir + "sprites/" + Resources_n::sprites[i] + ".png"))
          .setSmooth(true);
    for (int i = 0; i < Resources_n::shadersCount; i++) {
      shaders
          .acquire(Resources_n::shaders[i],
                   thor::Resources::fromFile<Shader>(
                       appDir + "shaders/" + Resources_n::shaders[i] + ".frag",
                       Shader::Type::Fragment))
          .setUniform("blur_radius", Vector2f(0.006f, 0.006f));
    }
    for (int i = 0; i < Resources_n::soundsCount; i++)
      soundBuffers.acquire(
          i, thor::Resources::fromFile<SoundBuffer>(
                 appDir + "sounds/" + Resources_n::sounds[i] + ".flac"));
    fonts.acquire(0,
                  thor::Resources::fromFile<Font>(appDir + "fonts/Tahoma.ttf"));
    if (!music.openFromFile(appDir + "sounds/" + Resources_n::music)) {
      throw thor::ResourceLoadingException("Failed to open" +
                                           Resources_n::music);
    }
  } catch (thor::ResourceLoadingException &e) {
    std::string message = e.what();
    while (message.find_first_of('\"') != std::string::npos) {
      message[message.find_first_of('\"')] = '*';
    }
    while (message.find_first_of('\'') != std::string::npos) {
      message[message.find_first_of('\'')] = '*';
    }
    tinyfd_messageBox("error", message.c_str(), "ok", "error", 1);
    std::cout << e.what() << std::endl;
    return 1;
  }
  std::filesystem::create_directory(std::filesystem::path("Screenshots"));
  Clock clk;
  sf::ContextSettings settings;
  settings.antialiasingLevel = 4;
  std::vector<VideoMode> modes = VideoMode::getFullscreenModes();
  std::unique_ptr<RenderWindow> window =
      std::make_unique<RenderWindow>(modes[modes.size() / 3], "BomberMan",
                                     Style::Titlebar | Style::Close, settings);
  sf::Image icon = textures["icon"].copyToImage();
  window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
  Color bg(10, 10, 10);
  window->setVerticalSyncEnabled(true);

  std::unique_ptr<Manager> manager = std::make_unique<Manager>(
      window.get(), bg, &textures["logo"], &shaders["blur"]);
  std::unique_ptr<Audio> audio = std::make_unique<Audio>(soundBuffers, &music);
  std::unique_ptr<Game> game =
      std::make_unique<Game>(*window, *audio, textures, fonts, CONTROL_HEIGHT);
  std::unique_ptr<Intro> intro =
      std::make_unique<Intro>(*window, bg, &textures["logo"], *manager, *audio);
  std::unique_ptr<Menu> menu = std::make_unique<Menu>(
      *window, bg, fonts[0], &textures["logo"], *manager, *audio);
  std::unique_ptr<Control> controls =
      std::make_unique<Control>(*window, *manager, *game, *audio,
                                window->getSize().x, CONTROL_HEIGHT, textures);
  manager->setPointers(intro.get(), menu.get(), controls.get(), game.get(),
                       audio.get(), &soundBuffers);

#ifdef DEBUGGING
  Clock timer;
  timer.restart();

  Text text;
  text.setFillColor(Color::Red);
  text.setFont(fonts[0]);
  text.setCharacterSize(20);
#endif // DEBUGGING
  while (window->isOpen()) {
    DeltaTime = clk.restart();
    sf::Event event;
    while (window->pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window->close();
      } else if (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::F2) {
        if (capture(*window))
          audio->play(Audio::Capture);
        else
          audio->play(Audio::Failed);
      } else if (event.type == Event::MouseMoved ||
                 event.type == sf::Event::MouseButtonPressed ||
                 event.type == Event::MouseButtonReleased ||
                 event.type == Event::KeyPressed ||
                 event.type == Event::KeyReleased) {
        manager->manageInput(event);
      }
    }

    window->clear(bg);
    manager->update(DeltaTime);

#ifdef DEBUGGING
    // if (manager->getState() == Manager::State::game) {
    Time t = timer.restart();
    text.setString("step: " + std::to_string(game->getTurn()) + "\t@(" +
                   std::to_string(Mouse::getPosition(*window).x) + ", " +
                   std::to_string(Mouse::getPosition(*window).y) +
                   ")\nFPS: " + std::to_string((int)(1 / t.asSeconds())));
    window->draw(text);
    //}

#endif // DEBUGGING
    window->display();
  }
  return 0;
}
