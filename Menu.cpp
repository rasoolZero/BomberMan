#include "Menu.h"

Menu::Menu(RenderWindow& window, Color background_color, Texture* logo_texture, Audio& audio)
	:window{window}
	, bg{ background_color }
	, logo_texture{ logo_texture }
	, audio{ audio }
	, middle{ static_cast<Vector2f>(window.getSize()) / 2.f }
{
	logo.setTexture(*logo_texture);
	logo.setOrigin(static_cast<Vector2f>(logo_texture->getSize()) / 2.f);
	logo.setScale(0.35f, 0.35f);
	logo.setPosition(middle.x, 0);
}

void Menu::update(Time DeltaTime)
{
	window.draw(logo);
}
