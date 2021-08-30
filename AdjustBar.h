#ifndef ADJUSTBAR_H
#define ADJUSTBAR_H
#include "SFML/Graphics.hpp"
using namespace sf;

class AdjustBar : public Drawable, public Transformable
{
public:
	AdjustBar(Vector2f size);
	void setTurnCount(int turnCount);
	void setProgress(float mouseX);
	double getProgress();
	void setValue(int value);
	int getValue();
	void advance(double turns);
	bool contains(Vector2f point);
private:
	RectangleShape base;
	RectangleShape bar;
	Vector2f size;
	double turn_width;
	int turn_count;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const; //from sf::Drawable
};

#endif // !ADJUSTBAR_H
