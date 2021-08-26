#include "Menu.h"
#include "tinyfiledialogs.h"
#include <json.hpp>
#include <cmath>
#include "macros.h"
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
		buttons[i].setFillColor(normal_button_color);
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
	info[Bomberman].setPosition(middle.x, 120);
	info[AICup].setPosition(middle.x, 165);
	info[file].setPosition(middle);
	info[error].setPosition(middle + Vector2f(0, 40));
}

void Menu::update(Time DeltaTime)
{
	if (info[error].getFillColor().a > 0) { // error fade
		message_timer += DeltaTime;
		if (message_timer >= message_display_time) {
			info[error].setFillColor(Color::Transparent);
		}
		else {
			Color temp = info[error].getFillColor();
			temp.a = (1 - pow(message_timer / message_display_time, 3)) * 255;
			info[error].setFillColor(temp);
		}
	} // error fade

	float progress = DeltaTime.asSeconds() / button_color_change_time.asSeconds();
	for (int i = 0; i < BUTTONCOUNT; i++)
	{
		Color current_color = buttons[i].getFillColor();
		if (i == selected) {
			if (current_color != selected_button_color) {
				current_color =  current_color - Color(color_difference.r * progress, color_difference.g * progress, color_difference.b * progress, 0);
				if (current_color.r < selected_button_color.r) {
					current_color.r = selected_button_color.r;
				}
				if (current_color.g < selected_button_color.g) {
					current_color.g = selected_button_color.g;
				}
				if (current_color.b < selected_button_color.b) {
					current_color.b = selected_button_color.b;
				}
				buttons[i].setFillColor(current_color);
			}
		}
		else {
			if (current_color != normal_button_color && (i != play || (i == play && !log_dir.empty() ))) {
				current_color =  current_color + Color(color_difference.r * progress, color_difference.g * progress, color_difference.b * progress, 0);
				if (current_color.r > normal_button_color.r) {
					current_color.r = normal_button_color.r;
				}
				if (current_color.g > normal_button_color.g) {
					current_color.g = normal_button_color.g;
				}
				if (current_color.b > normal_button_color.b) {
					current_color.b = normal_button_color.b;
				}
				buttons[i].setFillColor(current_color);
			}
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

void Menu::appear(double progress)
{
	if (progress == 0.0) {
		return;
	}
	for (int i = 0; i < BUTTONCOUNT; i++) {
		window.draw(buttons[i]);
	}
	for (int i = INFOCOUNT - 1; i >= 2; i--) {
		if (i != error || (i == error && info[error].getFillColor().a > 0)) {
			window.draw(info[i]);
		}
	}
	if (progress >= 0.7) {
		window.draw(info[1]);
		window.draw(info[0]);
	}
}

void Menu::manageKey(Event::KeyEvent key, bool released) {
	if (!released) {
		if (key.code == Keyboard::Key::Escape) {
			window.close();
		}
		else if (key.code == Keyboard::Key::Left) {
			selected = static_cast<Button>((selected + 1) % BUTTONCOUNT);
			if (selected == play && log_dir.empty()) {
				// skip play button if not available
				selected = static_cast<Button>((selected + 1) % BUTTONCOUNT);
			}
		}
		else if (key.code == Keyboard::Key::Right) {
			selected = static_cast<Button>((selected + BUTTONCOUNT - 1) % BUTTONCOUNT);
			if (selected == play && log_dir.empty()) {
				// skip play button if not available
				selected = static_cast<Button>((selected + BUTTONCOUNT - 1) % BUTTONCOUNT);
			}
		}
		else if (key.code == Keyboard::Key::Space || key.code == Keyboard::Key::Enter) {
			switch (selected)
			{
			case Menu::choose:
				chooseFile();
				break;
			case Menu::play:
			#ifdef DEBUGGING
				assert(!log_dir.empty());
			#endif // DEBUGGING
				load();
				break;
			default:
				break;
			}
		}
	}
	
}

void Menu::manageMouse(Event::MouseButtonEvent mouseButton, bool released)
{
	if (!released) {
		if (mouseButton.button == Mouse::Button::Left) {
			if (buttons[choose].getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
				chooseFile();
			}
			else if (buttons[play].getGlobalBounds().contains(mouseButton.x, mouseButton.y) && !log_dir.empty()) {
				load();
			}
		}
	}
}

void Menu::updateMouse(Event::MouseMoveEvent mouseMove)
{
	for (int i = 0; i < BUTTONCOUNT; i++)
	{
		if ((i != play || (i == play && !log_dir.empty())) && buttons[i].getGlobalBounds().contains(mouseMove.x, mouseMove.y)) {
			selected = static_cast<Button>(i);
			break;
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
	if (!log_dir.empty()) {
		buttons[play].setFillColor(normal_button_color);
	}
}

void Menu::load()
{
	try
	{
		manager.loadGame(log_dir);
		manager.setState(Manager::State::game);
	}
	catch (const nlohmann::json::exception& e)
	{
#ifdef DEBUGGING
		info[error].setString(e.what());
		info[error].setOrigin(info[error].getLocalBounds().width / 2, 0);
#endif // DEBUGGING
		info[error].setFillColor(Color::Red);
		message_timer = Time::Zero;
	}
}
