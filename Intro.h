#ifndef INTRO_H
#define INTRO_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Thor/Animations.hpp>
#include "CharShape.h"
#include "MoveAnimation.h"
#include "Mask.h"
#include "Manager.h"
#include "Audio.h"
using namespace sf;

class Intro
{
public:
	Intro(RenderWindow& window, Color background_color, Texture* logo_texture, Manager& manager, Audio& audio);
	void update(Time DeltaTime);
private:
	enum Piece {
		A, A_flipped, I, I_flipped, left_big, right_big, left_small, right_small
	};
	RenderWindow& window;
	Manager& manager;
	Audio& audio;
	Color bg;
	CharShape pieces[8];
	Vector2f gap;
	Vector2f middle;
	MoveAnimation l_move;
	MoveAnimation r_move;
	MoveAnimation l_parallel_move;
	MoveAnimation r_parallel_move; // extra move animations for right small and left small
	MoveAnimation logo_animation;
	thor::Animator<CharShape, std::string> l_animator;
	thor::Animator<CharShape, std::string> r_animator;
	thor::Animator<CharShape, std::string> l_parallel_animator;
	thor::Animator<CharShape, std::string> r_parallel_animator; // extra animators for right small and left small
	thor::Animator<Transformable, std::string> logo_animator;
	VertexArray logo;
	Transformable logo_transform;
	Texture* logo_texture;
	VertexArray logo_veil;
	VertexArray top_veil;
	VertexArray bot_veil; //two veil "layers" to hide parts of pieces
	Mask l_mask;
	Mask r_mask;
	void setNextVeil();
	void setNextAnimation();
	short active_piece = 0;
	bool A_appeared = false;
	Clock stall;
	Time delay;
	bool ready = true;
	void wait(Time delay);
	void checkDelay();
};

#endif // !INTRO_H
