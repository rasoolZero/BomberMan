#ifndef ADJUSTBAR_H
#define ADJUSTBAR_H
#include "SFML/Graphics.hpp"
using namespace sf;

class AdjustBar : public Drawable, public Transformable
{
public:
	AdjustBar(Vector2f size);
	void setTurnCount(int turnCount);
	void setValue(float mouseX);
	double getProgress();
	int getValue();
	void advance(double turns);
private:
	RectangleShape base;
	RectangleShape bar;
	Vector2f size;
	double turn_width;
	int turn_count;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const; //from sf::Drawable
};

#endif // !ADJUSTBAR_H
