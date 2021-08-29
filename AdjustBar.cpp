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
	bar.setSize(Vector2f(0, size.y));
}

void AdjustBar::setProgress(float mouseX)
{
	mouseX = mouseX - this->getPosition().x + turn_width / 3;
	if (mouseX < 0) {
		mouseX = 0;
	}
	else if (mouseX > size.x) {
		mouseX = size.x;
	}
	mouseX = static_cast<int>(mouseX / turn_width) * turn_width;
	bar.setSize(Vector2f(mouseX, size.y));
}

double AdjustBar::getProgress()
{
	return static_cast<double>(bar.getSize().x) / size.x;
}

void AdjustBar::setValue(int value)
{
	bar.setSize(Vector2f(turn_width * value, size.y));
}

int AdjustBar::getValue()
{
	return (bar.getSize().x + 0.5) / turn_width;
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

bool AdjustBar::contains(Vector2f point)
{
	return base.getGlobalBounds().contains(this->getTransform().getInverse() * point);
}

void AdjustBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();
	target.draw(base, states);
	target.draw(bar , states);
}
