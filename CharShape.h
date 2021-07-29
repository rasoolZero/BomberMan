#ifndef CharShape_h
#define CharShape_h

#include <SFML/Graphics.hpp>
#include <vector>
#include "Spline.hpp"
using namespace sf;

class CharShape : public Transformable
{
public:
	CharShape(char type, Color color, Vector2f wing_vector, float thickness);
	void setposition(Vector2f position);
	void move(Vector2f move_vector);
	sw::Spline& getSpline() { return this->spline; }
private:
	char type;
	std::vector<Vector2f> vertices;
	sw::Spline spline;
	Vector2f wing_vector;
	float thickness;
	//virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const; //from sf::Drawable
	void assign_points();
};

#endif // !CharShape_h

