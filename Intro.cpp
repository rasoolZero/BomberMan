#include "Intro.h"
#include "CharShape.h"
Intro::Intro(RenderWindow& window)
	:window{ window }
	,pieces{ 
		{'^', Color(0, 149,250), Vector2f(125, 217), 69}, //A
		{'^', Color(0, 149,250), Vector2f(125, 217), 69}, //A_flipped
		{'\\', Color(0, 149,250), Vector2f(125, 217), 69}, //I
		{'\\', Color(0, 149,250), Vector2f(125, 217), 69}, //I_flipped
		{'<', Color(245, 4, 126), Vector2f(131, 227), 69}, //left_big
		{'>', Color(245, 4, 126), Vector2f(131, 227), 69}, //right_big
		{'<', Color(255, 167, 0), Vector2f(95, 165), 69}, //left_small
		{'>', Color(255, 167, 0), Vector2f(95, 165), 69}  //right_small
	}
	, gap{ 21, 19 }
	, l_move{ {0, -(pieces[A].getWing().y + gap.y/2) }, MoveAnimation<CharShape>::Mode::ac_de }
	,r_move{ {0, pieces[A_flipped].getWing().y + gap.y/2 }, MoveAnimation<CharShape>::Mode::ac_de }
	, l_parallel_move{ {-(pieces[left_big].getThickness() + gap.x) * 2,0}, MoveAnimation<CharShape>::Mode::settle }
	, r_parallel_move{ {(pieces[right_big].getThickness() + gap.x) * 2,0}, MoveAnimation<CharShape>::Mode::settle }
{
	Vector2f middle = static_cast<Vector2f>(window.getSize()) / 2.f;
	
	pieces[A].setPosition({ middle.x - pieces[A].getWing().x * 2, middle.y });
	pieces[A_flipped].setPosition(pieces[A].getPosition());
	pieces[A_flipped].flip(middle);

	pieces[I].setPosition({middle.x - (pieces[A].getWing().x - gap.x / 2), middle.y - pieces[I].getWing().y - gap.y/2});
	pieces[I_flipped].setPosition(pieces[I].getPosition());
	pieces[I_flipped].flip(middle);

	//pieces[left_big].setPosition(pieces[A].getPosition() - pieces[left_big].getWing() + Vector2f{pieces[A].getThickness() + gap.x, 0});
	pieces[left_big].setPosition({ middle.x - pieces[A].getWing().x + gap.x/2 + pieces[A].getThickness() - pieces[left_big].getWing().x
		,middle.y - pieces[left_big].getWing().y });
	pieces[right_big].flip(middle);
	pieces[right_big].setPosition(pieces[left_big].getPosition());
	pieces[right_big].flip(middle);
	//pieces[right_big].setPosition({ middle.x - gap.x, middle.y - pieces[right_big].getWing().y });

	pieces[left_small].setPosition({ middle.x - (pieces[left_big].getThickness() * 2 + gap.x * (gap.y/gap.x) + gap.x), middle.y - pieces[left_small].getWing().y });
	pieces[right_small].flip(middle);
	pieces[right_small].setPosition(pieces[left_small].getPosition());
	pieces[right_small].flip(middle);


	l_animator.addAnimation("l", thor::refAnimation(l_move), seconds(.5));
	r_animator.addAnimation("r", thor::refAnimation(r_move), seconds(.5));
	l_animator.playAnimation("l");
	r_animator.playAnimation("r");

	l_parallel_animator.addAnimation("l", thor::refAnimation(l_parallel_move), seconds(.75));
	r_parallel_animator.addAnimation("r", thor::refAnimation(r_parallel_move), seconds(.75));
	l_parallel_animator.playAnimation("l");
	r_parallel_animator.playAnimation("r");
	frame_timer.restart();
	wait(seconds(1));
}

void Intro::update()
{
	if (active_piece == 6)/* &&
		(!l_parallel_animator.isPlayingAnimation() && !r_parallel_animator.isPlayingAnimation()))*/
		ready = false;
	if (ready) {
		l_animator.update(frame_timer.getElapsedTime());
		l_animator.animate(pieces[active_piece]);
		if (active_piece == 4) {
			l_parallel_animator.update(frame_timer.getElapsedTime());
			l_parallel_animator.animate(pieces[active_piece+2]);
			r_parallel_animator.update(frame_timer.getElapsedTime());
			r_parallel_animator.animate(pieces[active_piece + 3]);
		}
		r_animator.update(frame_timer.restart());
		r_animator.animate(pieces[active_piece+1]);


		if (!l_animator.isPlayingAnimation() && !r_animator.isPlayingAnimation() && active_piece != 4) {
			l_move(pieces[active_piece], 1.0);
			r_move(pieces[active_piece + 1], 1.0);
			setNextAnimation();
			/*if (1 <active_piece && active_piece < 6) {
				pieces[active_piece].setPosition(pieces[active_piece - 2].getPosition());
				pieces[active_piece + 1].setPosition(pieces[active_piece - 1].getPosition());
			}*/
			if (active_piece != 6) {
				l_animator.playAnimation("l");
				r_animator.playAnimation("r");
			}
			else {}
		}
	}
	else {
		checkDelay();
	}
	for (int i = 0; i < active_piece + (active_piece == 4 ? 4 :2); i++) {
		window.draw(pieces[i]);
	}
}

void Intro::setNextAnimation()
{
	switch (active_piece)
	{
	case 0:
		if (pieces[0].getPosition().x < window.getSize().x/2 - pieces[A].getWing().x * 2 + 1) {
			l_move.reset({ gap.x / 2 + pieces[A].getThickness(), 0 }, MoveAnimation<CharShape>::Mode::accelerate);
			r_move.reset({ -(gap.x / 2+ pieces[A_flipped].getThickness()), 0 }, MoveAnimation<CharShape>::Mode::accelerate);
		}
		else {
			l_move.reset({ gap.x + pieces[I].getThickness(), 0 }, MoveAnimation<CharShape>::Mode::decelerate);
			r_move.reset({ -(gap.x + pieces[I_flipped].getThickness()), 0 }, MoveAnimation<CharShape>::Mode::decelerate);
			active_piece += 2;
		}
		break;
	case 2:
		l_move.reset({ -(gap.x + pieces[left_big].getThickness()), 0 }, MoveAnimation<CharShape>::Mode::settle);
		r_move.reset({ gap.x + pieces[right_big].getThickness(), 0 }, MoveAnimation<CharShape>::Mode::settle);
		active_piece += 2;
		break;
	case 4:
		if (!l_parallel_animator.isPlayingAnimation() && !r_parallel_animator.isPlayingAnimation()) {
			active_piece += 2;
		}
		break;
	default:
		break;
	}
}

void Intro::wait(Time delay)
{
	ready = false;
	this->delay = delay;
	stall.restart();
}

void Intro::checkDelay()
{
	if (stall.getElapsedTime() > delay) {
		ready = true;
		delay = Time::Zero;
		frame_timer.restart();
	}
}
