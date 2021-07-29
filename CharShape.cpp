#include "CharShape.h"
#include <stdexcept>
CharShape::CharShape(char type, Color color, Vector2f wing_vector, float thickness)
	:type{ type }
	,wing_vector{ abs(wing_vector.x), abs(wing_vector.y) }
	,thickness{ thickness }
{
	spline.setColor(color);
	assign_points();
}

void CharShape::setposition(Vector2f position)
{
	Vector2f move_vector = position - this->getPosition();
	this->move(move_vector);
}

void CharShape::move(Vector2f move_vector)
{
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] += move_vector;
	}
	spline.setPositions(vertices);
	spline.update();
}

void CharShape::assign_points()
{
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
	spline.removeVertices(0);
	spline.addVertices(vertices);
	spline.update();
}
