#include <SFML/Graphics.hpp>
#include <iostream>
#include "Manager.h"
#include "Intro.h"
#include "Menu.h"
#include "Control.h"
#include "Resources_n.h"
#include "Audio.h"
#include "Game.h"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <Thor/Resources.hpp>
#include <memory>
#include "tinyfiledialogs.h"
#include "macros.h"
#include "json.hpp"
#define CONTROL_HEIGHT 20
using namespace sf;
namespace fs = std::filesystem;
using json = nlohmann::json;

VideoMode loadSettings(bool* fullscreen, std::string* default_dir);
void createSettings();

bool capture(sf::RenderWindow & window){
    Texture texture;
	if(!texture.create(window.getSize().x, window.getSize().y))
		return false;

    texture.update(window);
    Image img = texture.copyToImage();//window.capture();

    size_t maxFound = 0;
    for(const auto & entry : fs::directory_iterator("Screenshots")) {
		size_t number=0;
		std::filesystem::path pathOfEntry = entry.path();
		std::wstring s = pathOfEntry.filename().wstring();
		size_t t = s.find_first_of(L"0123456789");
		if(t!=std::wstring::npos){
			number = std::stoi(s.c_str()+t);
			if(number>maxFound)
				maxFound=number;
		}
    }
	img.saveToFile(std::string("Screenshots/Scr") + std::to_string(maxFound+1) + std::string(".png") );
    return true;
}

int main()
{
    Time DeltaTime;
    thor::ResourceHolder<Texture,std::string> textures;
    thor::ResourceHolder<Shader, std::string> shaders;
    thor::ResourceHolder<SoundBuffer,int> soundBuffers;
    thor::ResourceHolder<Font,int> fonts;
    Music music;
    try{
        for(int i=0;i<Resources_n::texturesCount;i++)
            textures.acquire(Resources_n::textures[i],thor::Resources::fromFile<Texture>("assets/sprites/"+Resources_n::textures[i]+".png")).setSmooth(true);
        textures["player1"].setSmooth(false);
        textures["player2"].setSmooth(false);
        for(int i=0;i<Resources_n::spritesCount;i++)
            textures.acquire(Resources_n::sprites[i],thor::Resources::fromFile<Texture>("assets/sprites/"+Resources_n::sprites[i]+".png")).setSmooth(true);
        for (int i = 0; i < Resources_n::shadersCount; i++) {
            shaders.acquire(Resources_n::shaders[i], thor::Resources::fromFile<Shader>("assets/shaders/" + Resources_n::shaders[i] + ".frag", Shader::Type::Fragment)).setUniform("blur_radius", Vector2f(0.006f,0.006f));
        }
        for(int i=0;i<Resources_n::soundsCount;i++)
            soundBuffers.acquire(i,thor::Resources::fromFile<SoundBuffer>("assets/sounds/"+Resources_n::sounds[i]+".flac"));
        fonts.acquire(0,thor::Resources::fromFile<Font>("assets/fonts/Tahoma.ttf"));
        if (!music.openFromFile("assets/sounds/" + Resources_n::music)) {
            throw thor::ResourceLoadingException("Failed to open" + Resources_n::music);
        }
    }
    catch(thor::ResourceLoadingException& e){
        std::string message = e.what();
        while (message.find_first_of('\"') != std::string::npos) {
            message[message.find_first_of('\"')] = '*';
        }
        while (message.find_first_of('\'') != std::string::npos) {
            message[message.find_first_of('\'')] = '*';
        }
        tinyfd_messageBox("error", message.c_str(), "ok", "error", 1);
        return 1;
    }
    std::filesystem::create_directory(std::filesystem::path("Screenshots"));
    Clock clk;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    VideoMode* mode = new VideoMode();
    std::string* default_dir = new std::string;
    bool is_fullscreen = false;
    try {
        if (!std::filesystem::exists("settings.json")) {
            createSettings();
        }
        *mode = loadSettings(&is_fullscreen, default_dir);
    }
    catch (std::ios_base::failure& e) {
        tinyfd_messageBox("error", e.what(), "ok", "error", 1);
        delete mode;
        return 1;
    }
    catch (json::exception& e) {
        createSettings();
        try {
            *mode = loadSettings(&is_fullscreen, default_dir);
        }
        catch (std::ios_base::failure& e) {
            tinyfd_messageBox("error", e.what(), "ok", "error", 1);
            delete mode;
            return 1;
        }
        catch (json::exception& _e) {
            std::string message = e.what();
            while (message.find_first_of('\"') != std::string::npos) {
                message[message.find_first_of('\"')] = '*';
            }
            while (message.find_first_of('\'') != std::string::npos) {
                message[message.find_first_of('\'')] = '*';
            }
            tinyfd_messageBox("failed to create settings", message.c_str(), "ok", "error", 1);
            delete mode;
            return 1;
        }
    }
    std::unique_ptr<RenderWindow> window = std::make_unique<RenderWindow>(*mode, "BomberMan", is_fullscreen ? Style::Fullscreen : Style::Close | Style::Titlebar, settings);
    delete mode;
    mode = nullptr;

	sf::Image icon = textures["icon"].copyToImage();
	window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    Color bg(10, 10, 10);
    window->setVerticalSyncEnabled(true);

    std::unique_ptr<Manager> manager = std::make_unique<Manager>(window.get(), bg, &textures["logo"], &shaders["blur"]);
    std::unique_ptr<Audio> audio = std::make_unique<Audio>(soundBuffers, &music);
    std::unique_ptr<Game> game = std::make_unique<Game>(*window,*audio,textures,fonts, CONTROL_HEIGHT);
    std::unique_ptr<Intro> intro = std::make_unique<Intro>(*window, bg, &textures["logo"], *manager, *audio);
    std::unique_ptr<Menu> menu = std::make_unique<Menu>(*window, bg, fonts[0], &textures["logo"], *manager, *audio, *default_dir);
    delete default_dir;
    default_dir = nullptr;
    std::unique_ptr<Control> controls = std::make_unique<Control>(*window, *manager, *game, *audio, window->getSize().x, CONTROL_HEIGHT, textures);
    manager->setPointers(intro.get(), menu.get(), controls.get(), game.get(), audio.get(), &soundBuffers);

    #ifdef DEBUGGING
    Clock timer;
    timer.restart();

    Text text;
    text.setFillColor(Color::Red);
    text.setFont(fonts[0]);
    text.setCharacterSize(20);
    #endif // DEBUGGING
    while (window->isOpen())
    {
        DeltaTime = clk.restart();
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2) {
                if(capture(*window))
					audio->play(Audio::Capture);
				else
					audio->play(Audio::Failed);
            }
            else if (event.type == Event::MouseMoved || event.type == sf::Event::MouseButtonPressed || event.type == Event::MouseButtonReleased
            || event.type == Event::KeyPressed || event.type == Event::KeyReleased) {
                manager->manageInput(event);
            }
        }

        window->clear(bg);
        manager->update(DeltaTime);
        #ifdef DEBUGGING
        //if (manager->getState() == Manager::State::game) {
            Time t = timer.restart();
            text.setString("step: " + std::to_string(game->getTurn()) + "\t@(" + std::to_string(Mouse::getPosition(*window).x) + ", " + std::to_string(Mouse::getPosition(*window).y) +")\nFPS: " + std::to_string((int)(1 / t.asSeconds())));
            window->draw(text);
        //}
        #endif // DEBUGGING
        window->display();
    }

    return 0;
}

VideoMode loadSettings(bool* fullscreen, std::string* default_dir)
{
    std::ifstream i;
    i.open("settings.json");
    if (!i.good()) {
        throw std::ios_base::failure("failed to load settings");
    }
    json settings;
    i >> settings;

    *fullscreen = false;
    *default_dir = settings["default_log_directory"];

#ifdef _WIN32
    if (settings["fullscreen"] == 1) {
        *fullscreen = true;
        return VideoMode::getDesktopMode();
    }
#endif // _WIN32
    unsigned width = settings["windowed_mode_width"];
    unsigned height = settings["windowed_mode_height"];
    return VideoMode(width, height);
}


void createSettings()
{
    std::ofstream o;
    if (!o.good()) {
        throw std::ios_base::failure("failed to create settings");
    }
    o.open("settings.json", std::ios_base::out | std::ios_base::trunc);
    std::string contents = "{\n\t";
#ifdef  _WIN32
    contents += "\"fullscreen\": 1, \n\t";
#endif //  _WIN32
    std::string dir = std::filesystem::current_path().string();
#ifdef _WIN32
    for (int i = dir.length() - 1; i >= 0; i--) {
        if (dir[i] == '\\') {
            dir.replace(i, 1, "\\\\");
        }
    }
#endif // _WIN32
    contents += "\"default_log_directory\": \"" + dir;
#ifdef _WIN32
    contents += "\\\\";
#else //unix
    contents += "/";
#endif // _WIN32
    contents += "\", \n\t";
    contents += "\"windowed_mode_width\": " + std::to_string(VideoMode::getFullscreenModes()[2].width)
        + ", \n\t\"windowed_mode_height\": " + std::to_string(VideoMode::getFullscreenModes()[2].height)
        + "\n}";
    o << contents;
}