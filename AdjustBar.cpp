#include "AdjustBar.h"

AdjustBar::AdjustBar(Vector2f size)
	:size{ size }
	, base{ size }
	, bar{ {0, size.y} }
{
	//this->turn_count = turn_count;
	//this->turn_width = static_cast<double>(size.x) / turn_count;

	base.setFillColor(Color(100, 100, 100, 100));
	base.setOutlineThickness(2);
	base.setOutlineColor(Color::Black);

	bar.setSize(Vector2f(0, size.y));
	bar.setFillColor(Color(220, 220, 220, 170));
}

void AdjustBar::setTurnCount(int turnCount)
{
	this->turn_count = turnCount;
	this->turn_width = static_cast<double>(size.x) / turn_count;
}

void AdjustBar::setValue(float mouseX)
{
	mouseX = mouseX - this->getPosition().x;
	if (mouseX < 0) {
		mouseX = 0;
	}
	else if (mouseX > size.x) {
		mouseX = size.x;
	}
	bar.setSize(Vector2f(mouseX, size.y));
}

double AdjustBar::getProgress()
{
	return static_cast<double>(bar.getSize().x) / size.x;
}

int AdjustBar::getValue()
{
	return bar.getSize().x / turn_width;
}

void AdjustBar::advance(double turns)
{
	bar.setSize(Vector2f(bar.getSize().x + turns * turn_width, size.y));
	if (bar.getSize().x < 0) {
		bar.setSize(Vector2f(0, size.y));
	}
	else if (bar.getSize().x > size.x) {
		bar.setSize(size);
	}
}

void AdjustBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();
	target.draw(base, states);
	target.draw(bar , states);
}
