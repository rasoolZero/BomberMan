#ifndef INTRO_H
#define INTRO_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Thor/Animations.hpp>
#include "CharShape.h"
#include "MoveAnimation.h"
using namespace sf;

class Intro
{
public:
	Intro(RenderWindow& window);
	void update();
private:
	enum Piece {
		A, A_flipped, I, I_flipped, left_big, right_big, left_small, right_small
	};
	RenderWindow& window;
	CharShape pieces[8];
	Vector2f gap;
	MoveAnimation l_move;
	MoveAnimation r_move;
	MoveAnimation l_parallel_move;
	MoveAnimation r_parallel_move; // extra move animations for right small and left small
	thor::Animator<CharShape, std::string> l_animator;
	thor::Animator<CharShape, std::string> r_animator;
	thor::Animator<CharShape, std::string> l_parallel_animator;
	thor::Animator<CharShape, std::string> r_parallel_animator; // extra animators for right small and left small

	void setNextAnimation();
	short active_piece = 0;
	Clock frame_timer;
	Clock stall;
	Time delay;
	bool ready = true;
	void wait(Time delay);
	void checkDelay();
};

#endif // !INTRO_H
