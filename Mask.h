#ifndef MASK_H
#define MASK_H
#include <SFML/Graphics.hpp>
#include <Thor/Animations.hpp>
#include "MoveAnimation.h"
using namespace sf;
class Mask : public Drawable
{
public:
	Mask(Vector2f source, Vector2f destination, Time duration, Vector2f wing_vector, Color color, Color background_color);
	void update(Time elapsed);
	Vector2f getPosition(short index = 2);
	float getProgress() { return this->move.getProgress(); }
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void setPosition(Vector2f position);
	Vector2f source;
	Vector2f movement_vector;
	Vector2f wing_vector;
	VertexArray vertices;
	//VertexArray glow;
	VertexArray front_glow;
	VertexArray back_glow;
	VertexArray fill;
	Color color;
	MoveAnimation move;
	thor::Animator<Mask, std::string> animator;

	friend class MoveAnimation;
};

#endif // !MASK_H


