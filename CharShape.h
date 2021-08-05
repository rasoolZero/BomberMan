#ifndef CHARSHAPE_h
#define CHARSHAPE_h

#include <SFML/Graphics.hpp>
#include <vector>
//#include "Spline.hpp"
#include <Thor/Shapes/ConcaveShape.hpp>
using namespace sf;

class CharShape : public Drawable
{
public:
	CharShape() {};
	CharShape(char type, Color color, Vector2f wing_vector, float thickness, Color background_color);
	void setPosition(Vector2f position);
	void move(Vector2f offset);
	Vector2f getPosition() { return this->shape.getPosition(); }
	Vector2f getPoint(int index) { return this->shape.getPoint(index); }
	const Transform& getTransform() { return this->shape.getTransform(); }
	//void move(Vector2f move_vector);
	//sw::Spline& getSpline() { return this->spline; }
	Vector2f getWing() { return this->wing_vector; }
	float getThickness() { return this->thickness; }
	void flip(Vector2f origin);
private:
	char type;
	//std::vector<Vector2f> vertices;
	//sw::Spline spline;
	thor::ConcaveShape shape;
	Vector2f wing_vector;
	float thickness;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const; //from sf::Drawable
	void assign_points();
};

#endif // !CHARSHAPE_h

