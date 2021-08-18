#include "Menu.h"
#include "tinyfiledialogs.h"
#include <json.hpp>
#include <cmath>
#define DEBUGGING
Menu::Menu(RenderWindow& window, Color background_color, Font& font, Texture* logo_texture, Manager& manager, Audio& audio)
	:window{window}
	, bg{ background_color }
	, logo_texture{ logo_texture }
	, manager{ manager }
	, audio{ audio }
	, middle{ static_cast<Vector2f>(window.getSize()) / 2.f }
{
	logo.setTexture(*logo_texture);
	logo.setOrigin(static_cast<Vector2f>(logo_texture->getSize()) / 2.f);
	logo.setScale(0.35f, 0.35f);
	logo.setPosition(middle.x, 0);

	for (int i = 0; i < BUTTONCOUNT; i++) {
		buttons[i].setCharacterSize(24);
		buttons[i].setFont(font);
		buttons[i].setPosition(middle + Vector2f(-110 + 220 * i, 90));
	}
	buttons[choose].setString(" Choose file ");
	buttons[play].setString(" Play ");
	buttons[play].setFillColor(Color(140, 140, 140));
	for (int i = 0; i < BUTTONCOUNT; i++) {
		buttons[i].setOrigin(buttons[i].getLocalBounds().width / 2, 0);
	}

	for (int i = 0; i < INFOCOUNT; i++) {
		info[i].setFont(font);
		info[i].setCharacterSize(38 - 3 * i);
	}
	info[AICup].setString("AICup");
	info[AICup].setFillColor(Color(150, 150, 150));
	info[Bomberman].setString("Bomberman");
	info[file].setString("No file chosen");
	info[error].setString("Invalid file. try choosing a valid JSON file");
	info[error].setFillColor(Color::Transparent);
	for (int i = 0; i < INFOCOUNT; i++) {
		info[i].setOrigin(info[i].getLocalBounds().width / 2, 0);
	}
	info[Bomberman].setPosition(middle - Vector2f(0, middle.y * 0.78));
	info[AICup].setPosition(middle - Vector2f(0, middle.y * 0.78 - 45));
	info[file].setPosition(middle);
	info[error].setPosition(middle + Vector2f(0, 40));
}

void Menu::update(Time DeltaTime)
{
	if (info[error].getFillColor().a > 0) {
		message_timer += DeltaTime;
		if (message_timer >= message_display_time) {
			info[error].setFillColor(Color::Transparent);
		}
		else {
			Color temp = info[error].getFillColor();
			temp.a = (1 - pow(message_timer / message_display_time, 3)) * 255;
			info[error].setFillColor(temp);
		}
	}
	window.draw(logo);
	for (int i = 0; i < BUTTONCOUNT; i++) {
		window.draw(buttons[i]);
	}
	for (int i = 0; i < INFOCOUNT; i++) {
		if (i != error || (i == error && info[error].getFillColor().a > 0)) {
			window.draw(info[i]);
		}
	}
}

void Menu::manageKey(Event::KeyEvent key, bool released) {
	if (key.code == Keyboard::Key::Escape && !released) {
		window.close();
	}
}

void Menu::manageMouse(Event::MouseButtonEvent mouseButton, bool released)
{
	if (!released) {
		if (mouseButton.button == Mouse::Button::Left) {
			if (buttons[choose].getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
				chooseFile();
				if (!log_dir.empty()) {
					buttons[play].setFillColor(Color::White);
				}
			}
			else if (buttons[play].getGlobalBounds().contains(mouseButton.x, mouseButton.y) && !log_dir.empty()) {
				try
				{
					manager.loadGame(log_dir);
					manager.setState(Manager::State::game);
				}
				catch (const nlohmann::json::exception& e)
				{
				#ifdef DEBUGGING
					info[error].setString(e.what());
				#endif // DEBUGGING
					info[error].setFillColor(Color::Red);
					info[error].setOrigin(info[error].getLocalBounds().width / 2, 0);
					message_timer = Time::Zero;
				}
			}
		}
	}
}

void Menu::chooseFile()
{
	char* temp_chr;
	std::string temp_str;
	temp_chr = tinyfd_openFileDialog("select a log file", "", 1, fileTypes, "JSON files", 0);
	if (temp_chr != NULL) {
		temp_str = temp_chr;
		//free(temp_chr); //memory leak?
		log_dir = temp_str;
	#ifdef _WIN32
		temp_str.erase(0, temp_str.find_last_of('\\') + 1);
	#else //unix
		temp_str.erase(0, temp_str.find_last_of('/') + 1);
	#endif
		info[file].setString(temp_str);
		info[file].setOrigin(info[file].getLocalBounds().width / 2, 0);
	}
}
