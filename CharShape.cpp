#include "CharShape.h"
#include <stdexcept>
CharShape::CharShape(char type, Color color, Vector2f wing_vector, float thickness, Color background_color = Color::Black)
	:type{ type }
	,wing_vector{ abs(wing_vector.x), abs(wing_vector.y) }
	,thickness{ thickness }
{
	assign_points();
	shape.setFillColor(background_color);
	shape.setOutlineColor(color);
	shape.setOutlineThickness(4);
	/*spline.setColor(color);
	assign_points(true);
	spline.setThickness(4);
	spline.setClosed(true);
	spline.setThickCornerType(sw::Spline::ThickCornerType::Round);
	spline.smoothHandles();
	spline.update();*/
	
}

void CharShape::setPosition(Vector2f position)
{
	/*Vector2f move_vector = position - this->getPosition();
	Transformable::setPosition(position);
	this->move(move_vector);*/
	this->shape.setPosition(position);
}

void CharShape::move(Vector2f offset)
{
	this->shape.setPosition(this->shape.getPosition() + offset);
}

/*void CharShape::move(Vector2f move_vector)
{
	/*for (int i = 0; i < vertices.size(); i++) {
		vertices[i] += move_vector;
	}
	spline.setPositions(vertices);
	spline.update();
}*/

void CharShape::flip(Vector2f origin)
{
	shape.setOrigin(origin - shape.getPosition());
	shape.move(origin - shape.getPosition());
	shape.rotate(180);
	/*this->spline.rotate(180.f, origin);
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] = spline.getPosition(i);
	}
	if (vertices[0].y > vertices[1].y) {
		if (type == '<' || type == '>') {
			Transformable::setPosition(vertices[0] - Vector2f(0, wing_vector.y));
		}
		else {
			Transformable::setPosition(vertices[1]);
		}
	}
	else {
		if (type == '<' || type == '>') {
			Transformable::setPosition(vertices[4]);
		}
		else {
			Transformable::setPosition(vertices[0] - Vector2f(wing_vector.x, 0));
		}
	}
	this->spline.update();*/
}

void CharShape::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//target.draw(spline, states);
	target.draw(shape, states);
}

void CharShape::assign_points()
{
	std::vector<Vector2f> vertices;
	if (type == '^') {
		vertices.push_back({ wing_vector.x, 0 });
		vertices.push_back({ wing_vector.x * 2, wing_vector.y });
		vertices.push_back({ wing_vector.x * 2 - thickness, wing_vector.y });
		vertices.push_back({ wing_vector.x, wing_vector.y * (thickness / wing_vector.x) });
		vertices.push_back({ thickness, wing_vector.y });
		vertices.push_back({ 0, wing_vector.y });
	}
	else if (type == '\\') {
		vertices.push_back({ thickness, 0 });
		vertices.push_back({ thickness + wing_vector.x, wing_vector.y });
		vertices.push_back(wing_vector);
		vertices.push_back({ 0, 0 });
	}
	else if (type == '<') {
		vertices.push_back({ 0, wing_vector.y });
		vertices.push_back({ wing_vector.x, 0 });
		vertices.push_back({ wing_vector.x + thickness, 0 });
		vertices.push_back({ thickness, wing_vector.y });
		vertices.push_back({ wing_vector.x + thickness, wing_vector.y * 2 });
		vertices.push_back({ wing_vector.x, wing_vector.y * 2 });
	}
	else if (type == '>') {
		vertices.push_back({ thickness + wing_vector.x, wing_vector.y });
		vertices.push_back({ thickness, wing_vector.y * 2 });
		vertices.push_back({ 0, wing_vector.y * 2 });
		vertices.push_back(wing_vector);
		vertices.push_back({0, 0});
		vertices.push_back({thickness, 0});
	}
	else {
		throw std::invalid_argument("invalid character");
	}
	shape.setPointCount(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		shape.setPoint(i, vertices[i]);
	}
}
