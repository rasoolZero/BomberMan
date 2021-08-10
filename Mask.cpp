#include "Mask.h"
#include <cmath>
#include <Thor/Math/Trigonometry.hpp>
// cbrt(1.512 * progress - 1) / 1.8 + 0.5555
// cbrt(2 * progress - 1.5) /1.94 +0.59
// pow (progress, 3)

Mask::Mask(Vector2f source, Vector2f destination, Time duration, Vector2f wing_vector, Color color, Color background_color)
	:color{ color }
	, source{ source }
	, movement_vector{ destination - source }
	, wing_vector{ wing_vector }
	, move{ movement_vector, MoveAnimation::Mode::ac_de }
{
	fill.resize(6);
	fill.setPrimitiveType(Triangles);

	this->vertices.setPrimitiveType(TriangleStrip);
	this->vertices.resize(6);
	for (int i = 0; i < 6; i++) {
		this->vertices[i].position = source;
		this->vertices[i].color = color;
	}
	this->front_glow.setPrimitiveType(TriangleStrip);
	this->back_glow.setPrimitiveType(TriangleStrip);
	this->front_glow.resize(6);
	this->back_glow.resize(6);
	for (int i = 0; i < front_glow.getVertexCount(); i++) {
		this->front_glow[i].position = source;
		this->back_glow[i].position = source;
		if (i % 2 == 0) {
			this->front_glow[i].color = color;
			this->back_glow[i].color = color;
		}
		else {
			this->front_glow[i].color = Color::Transparent;
			this->back_glow[i].color = Color::Transparent;
		}
	}
	animator.addAnimation("move",thor::refAnimation(move), duration);
	animator.playAnimation("move");
}

void Mask::update(Time elapsed)
{
	animator.update(elapsed);
	animator.animate(*this);
	if (!animator.isPlayingAnimation()) {
		move(*this, 1.0);
	}
}

Vector2f Mask::getPosition(short index)
{
	return this->vertices[index].position; 
}

void Mask::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(vertices, states);
	target.draw(fill, states);
	if (move.getProgress() != 1.0) {
		//target.draw(front_glow, states);
		//target.draw(back_glow, states);
	}
}

void Mask::setPosition(Vector2f position)
{
	if (move.getProgress() == 0.0) {
		for (int i = 0; i < vertices.getVertexCount(); i++) {
			this->vertices[i].position = position;
			this->front_glow[i].position = position;
			this->back_glow[i].position = position;
		}
	}
	else {
		Vector2f wing = wing_vector;// *static_cast<float>(((cbrt(2 * move.getProgress() - 1) + 1) / 2) - (cbrt(2 * move.getProgress() - 1.5) / 1.94 + 0.59));
		if (move.getProgress() == 1.f) {
			if (vertices[2].position.x > vertices[3].position.x) {
				vertices[2].position = vertices[3].position = position;
				vertices[0].position = vertices[1].position = position + Vector2f(wing.x, -wing.y);
				vertices[4].position = vertices[5].position = position + wing;
			}
			else if(vertices[2].position.x < vertices[3].position.x) {
				vertices[2].position = vertices[3].position = position;
				vertices[0].position = vertices[1].position = position + wing;
				vertices[4].position = vertices[5].position = position + Vector2f(wing.x, -wing.y);
			}
			return;
		}
		vertices[2].position = position;
		vertices[3].position = source + movement_vector * static_cast<float>(pow(move.getProgress(), 3));
		//vertices[3].position = source + movement_vector * static_cast<float>(cbrt(2 * move.getProgress() - 1.5) / 1.94 + 0.59);
		Vector2f difference = vertices[2].position - vertices[3].position; // head - tail
		double distance = sqrt(pow(difference.x, 2) + pow(difference.y, 2));
		Transform rebase;
		// rotate the wing vector by degrees equal to degrees between x axis and difference
		rebase.rotate(thor::toDegree(acos(-difference.x / distance)));
		Vector2f rebased_r = rebase.transformPoint(wing);
		Vector2f rebased_l = rebase.transformPoint(wing.x, -wing.y);
		//vertices[0].position = vertices[2].position + rebased_r; //add the rebased wing vector to position of head
		//vertices[4].position = vertices[2].position + rebased_l;
		float travelled = abs(position.x - source.x);
		if (abs(rebased_l.x) > travelled) {
 			vertices[0].position = vertices[2].position + rebased_r * static_cast<float>(travelled / abs(rebased_r.x));
			vertices[4].position = vertices[2].position + rebased_l * static_cast<float>(travelled / abs(rebased_l.x));
		}
		else {
			vertices[0].position = vertices[2].position + rebased_r;
			vertices[4].position = vertices[2].position + rebased_l;
		}
		if (abs(vertices[0].position.x - source.x) >= abs(difference.x)) {
		//if(move.getProgress() > 0.4) {
			vertices[1].position = vertices[3].position + rebased_r;
			vertices[5].position = vertices[3].position + rebased_l;
		}
		else {
			vertices[1].position = vertices[3].position + rebased_r * static_cast<float>((travelled - abs(difference.x)) / abs(rebased_r.x));
			vertices[5].position = vertices[3].position + rebased_l * static_cast<float>((travelled - abs(difference.x)) / abs(rebased_l.x));
			//vertices[1].position = { vertices[0].position.x - (vertices[0].position.x - source.x), vertices[0].position.y };
			//vertices[5].position = { vertices[4].position.x - (vertices[0].position.x - source.x), vertices[4].position.y };
		}

		fill[0] = vertices[0];
		fill[1] = vertices[1];
		fill[2].position = { source.x , vertices[0].position.y };
		fill[2].color = color;
		fill[3] = vertices[4];
		fill[4] = vertices[5];
		fill[5].position = { source.x , vertices[4].position.y };

		for (int i = 0; i < 6; i += 2) {
			front_glow[i].position = vertices[i].position;
			back_glow[i].position = vertices[i+1].position;
			front_glow[i+1].position = vertices[i].position + difference / 4.f;
			back_glow[i+1].position = vertices[i+1].position - difference / 3.f;
		}
		/*glow[0].position = vertices[0].position;
		glow[2].position = vertices[1].position;
		glow[4].position = vertices[3].position;
		glow[6].position = vertices[5].position;
		glow[8].position = vertices[4].position;
		glow[10].position = vertices[2].position;

		glow[11].position = vertices[2].position + difference / 4.f; //head
		glow[5].position = vertices[3].position - difference / 2.f; //tail
		if ((rebased_r * 1.25f).x > travelled) {
			glow[1].position = glow[5].position + rebased_r * 1.25f * travelled / abs(rebased_r.x);
			glow[9].position = glow[5].position + rebased_l * 1.25f * travelled / abs(rebased_r.x);
		}
		else {
			glow[1].position = glow[5].position + rebased_r * 1.25f;
			glow[9].position = glow[5].position + rebased_l * 1.25f;
		}
		//glow[1].position = glow[5].position + rebased_r * 1.25f * ((rebased_r * 1.25f).x >= abs(position.x - source.x) ? 1 : (position.x - source.x) / rebased_r.x);
		glow[3].position = glow[1].position - difference * 1.75f;
		glow[7].position = glow[9].position - difference * 1.75f;*/
	}
}
