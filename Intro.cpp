#include "Intro.h"
#include "CharShape.h"
#include <stdexcept>
Intro::Intro(RenderWindow& window, Color background_color, Texture* logo_texture, Manager& manager, Audio& audio)
	:window{ window }
	,manager{manager}
	,audio{ audio }
	,bg{background_color}
	,logo_texture{logo_texture}
	,pieces{
		{'^', Color(0, 149,250), Vector2f(125, 217), 69, bg}, //A
		{'^', Color(0, 149,250), Vector2f(125, 217), 69, bg}, //A_flipped
		{'\\', Color(0, 149,250), Vector2f(125, 217), 69, bg}, //I
		{'\\', Color(0, 149,250), Vector2f(125, 217), 69, bg}, //I_flipped
		{'<', Color(245, 4, 126), Vector2f(131, 227), 69, bg}, //left_big
		{'>', Color(245, 4, 126), Vector2f(131, 227), 69, bg}, //right_big
		{'<', Color(255, 167, 0), Vector2f(95, 165), 69, bg}, //left_small
		{'>', Color(255, 167, 0), Vector2f(95, 165), 69, bg}  //right_small
	}
	, gap{ 21, 19 }
	, middle{ static_cast<Vector2f>(window.getSize()) / 2.f }
	, l_move{ {0, -(pieces[A].getWing().y + gap.y/2) }, MoveAnimation::Mode::ac_de }
	,r_move{ {0, pieces[A_flipped].getWing().y + gap.y/2 }, MoveAnimation::Mode::ac_de }
	, l_parallel_move{ {-(pieces[left_big].getThickness() + gap.x) * 2,0}, MoveAnimation::Mode::settle }
	, r_parallel_move{ {(pieces[right_big].getThickness() + gap.x) * 2,0}, MoveAnimation::Mode::settle }
	, logo_animation{ { 0, -middle.y }, MoveAnimation::Mode::uniform }
	, l_mask{ middle, middle - Vector2f{400, 0}, seconds(1.75), Vector2f(pieces[left_big].getWing().x, -pieces[left_big].getWing().y) * 1.1f, Color::White, bg }
	, r_mask{ middle, middle + Vector2f{400, 0}, seconds(1.75), Vector2f(pieces[right_big].getWing().x, -pieces[right_big].getWing().y) * 1.1f, Color::White, bg }
{
	top_veil.setPrimitiveType(TriangleStrip);
	bot_veil.setPrimitiveType(TriangleStrip);

	logo.resize(6);
	logo.setPrimitiveType(TriangleStrip);
	logo_veil.resize(6);
	logo_veil.setPrimitiveType(TriangleStrip);
	for (int i = 0; i < logo_veil.getVertexCount(); i++) {
		logo_veil[i].color = bg;
	}
	logo_animator.addAnimation("logo", thor::refAnimation(logo_animation), seconds(1.5f));
	logo_transform.setOrigin(middle);
	logo_transform.setPosition(middle);

	pieces[A].setPosition({ middle.x - pieces[A].getWing().x * 2, middle.y });
	pieces[A_flipped].setPosition(pieces[A].getPosition());
	pieces[A_flipped].flip(middle);

	pieces[I].setPosition({middle.x - (pieces[A].getWing().x - gap.x / 2), middle.y - pieces[I].getWing().y - gap.y/2});
	pieces[I_flipped].setPosition(pieces[I].getPosition());
	pieces[I_flipped].flip(middle);

	//pieces[left_big].setPosition(pieces[A].getPosition() - pieces[left_big].getWing() + Vector2f{pieces[A].getThickness() + gap.x, 0});
	pieces[left_big].setPosition({ middle.x - pieces[A].getWing().x + gap.x/2 + pieces[A].getThickness() - pieces[left_big].getWing().x
		,middle.y - pieces[left_big].getWing().y });
	pieces[right_big].setPosition(pieces[left_big].getPosition());
	pieces[right_big].move( { ( middle.x - pieces[right_big].getPosition().x)*2 - (pieces[right_big].getThickness() + pieces[right_big].getWing().x), 0 });
	/*pieces[right_big].flip(middle);
	pieces[right_big].flip(middle);*/
	//pieces[right_big].setPosition({ middle.x - gap.x, middle.y - pieces[right_big].getWing().y });

	pieces[left_small].setPosition({ middle.x - (pieces[left_big].getThickness() * 2 + gap.x * (gap.y/gap.x) + gap.x), middle.y - pieces[left_small].getWing().y });
	pieces[right_small].setPosition(pieces[left_small].getPosition());
	pieces[right_small].move({ (middle.x - pieces[right_small].getPosition().x) * 2 - (pieces[right_small].getThickness() + pieces[right_small].getWing().x) , 0 });
	/*pieces[right_small].flip(middle);
	pieces[right_small].flip(middle);*/


	l_animator.addAnimation("l", thor::refAnimation(l_move), seconds(.4f));
	r_animator.addAnimation("r", thor::refAnimation(r_move), seconds(.4f));
	l_animator.playAnimation("l");
	r_animator.playAnimation("r");

	l_parallel_animator.addAnimation("l", thor::refAnimation(l_parallel_move), seconds(.6f));
	r_parallel_animator.addAnimation("r", thor::refAnimation(r_parallel_move), seconds(.6f));
	//l_parallel_animator.playAnimation("l");
	//r_parallel_animator.playAnimation("r");

	setNextVeil();
	//audio.play(Audio::Sounds::Intro);
	/*if (!logo_texture.loadFromFile("./assets/sprites/logo.png")) {
		throw std::runtime_error("couldnt load logo");
	}
	logo_texture.setSmooth(true);*/

	wait(seconds(0.75f));
}

void Intro::update(Time DeltaTime)
{
	/*if (active_piece <= 6)/* &&
		(!l_parallel_animator.isPlayingAnimation() && !r_parallel_animator.isPlayingAnimation()))
	{
		//ready = false;
		l_mask.update(frame_timer.getElapsedTime());
		r_mask.update(frame_timer.restart());

	}*/
	if (ready) {
		if (active_piece >= 6)/* &&
		(!l_parallel_animator.isPlayingAnimation() && !r_parallel_animator.isPlayingAnimation()))*/
		{
			//ready = false;
			l_mask.update(DeltaTime);
			logo_animator.update(DeltaTime);
			logo_animator.animate(logo_transform);
			r_mask.update(DeltaTime);
			logo_transform.setScale(Vector2f(1.f, 1.f) - Vector2f(.5f, .5f) * static_cast<float>(logo_animation.getProgress()));
			if (l_mask.getProgress() == 1.f && r_mask.getProgress() == 1.f) {
				if (active_piece == 6) {
					logo_animator.playAnimation("logo");
					wait(seconds(1.5f));
				}
				active_piece = 8;
			}
			if (active_piece == 8 && !logo_animator.isPlayingAnimation()) { //end of intro
				logo_animation(logo_transform, 1.f);
				manager.setState(Manager::State::menu);
			}
		}
		else {
			l_animator.update(DeltaTime);
			l_animator.animate(pieces[active_piece]);
			if (active_piece == 4) {
				l_parallel_animator.update(DeltaTime);
				l_parallel_animator.animate(pieces[active_piece + 2]);
				r_parallel_animator.update(DeltaTime);
				r_parallel_animator.animate(pieces[active_piece + 3]);
			}
			r_animator.update(DeltaTime);
			r_animator.animate(pieces[active_piece + 1]);


			if (!l_animator.isPlayingAnimation() && !r_animator.isPlayingAnimation() /*&& active_piece != 4*/) {
				l_move(pieces[active_piece], 1.0);
				r_move(pieces[active_piece + 1], 1.0);
				setNextAnimation();
				/*if (!l_parallel_animator.isPlayingAnimation() && !r_parallel_animator.isPlayingAnimation()) {
					//setNextVeil();
				}
				/*if (1 <active_piece && active_piece < 6) {
					pieces[active_piece].setPosition(pieces[active_piece - 2].getPosition());
					pieces[active_piece + 1].setPosition(pieces[active_piece - 1].getPosition());
				}*/
				/*if (active_piece != 4) {
					l_animator.playAnimation("l");
					r_animator.playAnimation("r");
				}*/
				if (active_piece == 6) {
					l_parallel_move(pieces[left_small], 1.0);
					r_parallel_move(pieces[right_small], 1.0);
					wait(seconds(.5f));
				}
			}
		}

	}
	else {
		checkDelay();
	}
	if (active_piece <= 6) {
		for (int i = active_piece + (active_piece == 4 ? 3 : 1); i >= 0; i--) {
			window.draw(pieces[i]);
			if ((i == active_piece) && (active_piece != 0 || (active_piece == 0 && !A_appeared))) {
				window.draw(top_veil);
				window.draw(bot_veil);
			}
		}
	}

	if (active_piece >= 6) {
		for (int i = 0; i < 6; i++) {
			if (i % 2) {
				logo[i].position = r_mask.getPosition(6 - i);

			}
			else {
				logo[i].position = l_mask.getPosition(i + 1);
			}
			logo_veil[i].position = logo[i].position;
			logo[i].texCoords = Vector2f((*logo_texture).getSize().x / 2.f, (*logo_texture).getSize().y / 2.f) + (logo[i].position - middle)*(11.2f/8.f);
		}
		window.draw(logo_veil, logo_transform.getTransform());
		window.draw(logo, RenderStates(BlendAlpha, logo_transform.getTransform(), logo_texture, nullptr));
		window.draw(l_mask);
		window.draw(r_mask);

	}
}

void Intro::setNextVeil()
{
	//Vector2f middle = static_cast<Vector2f>(window.getSize()) / 2.f;
	CharShape p;
	switch (active_piece)
	{
	case 0:
		top_veil.resize(6);
		bot_veil.resize(6);
		p = pieces[A_flipped];
		if (!A_appeared) {
			top_veil[0].position = p.getTransform().transformPoint(p.getPoint(1)) - Vector2f(10, 10);
			top_veil[1].position = p.getTransform().transformPoint(p.getPoint(5)) + Vector2f(10, -10);
			top_veil[2].position = { p.getTransform().transformPoint(p.getPoint(0)).x - (p.getWing().x + 5), middle.y + 2 };
			top_veil[3].position = { p.getTransform().transformPoint(p.getPoint(0)).x + (p.getWing().x + 5), middle.y + 2 };
			top_veil[4].position = top_veil[2].position + Vector2f(0, gap.y / 4);
			top_veil[5].position = top_veil[3].position + Vector2f(0, gap.y / 4);

			for (int i = 0; i < 6; i++) {
				bot_veil[i].position = 2.f * middle - top_veil[i].position; //mirror positions based on middle
			}
		}
		break;
	case 2:
		top_veil.resize(6);
		bot_veil.resize(6);
		p = pieces[A_flipped];

		bot_veil[0].position = p.getTransform().transformPoint(p.getPoint(5)) - Vector2f(0,5);
		bot_veil[1].position = bot_veil[0].position + Vector2f(0, p.getWing().y + 10);
		bot_veil[2].position = p.getTransform().transformPoint(p.getPoint(1)) + Vector2f(-2, -5);
		bot_veil[3].position = p.getTransform().transformPoint(p.getPoint(0)) + Vector2f(-2, 5);
		bot_veil[4].position = bot_veil[2].position - Vector2f(gap.x / 2, 0);
		bot_veil[5].position = bot_veil[3].position - Vector2f(gap.x / 2, 0);

		for (int i = 0; i < 6; i++) {
			top_veil[i].position = 2.f * middle - bot_veil[i].position;
		}
		break;
	case 4:
		top_veil.resize(8);
		bot_veil.resize(8);
		p = pieces[left_big];

		top_veil[2].position = p.getTransform().transformPoint(p.getPoint(1)) - Vector2f(2, 5);
		top_veil[3].position = p.getTransform().transformPoint(p.getPoint(0)) - Vector2f(2, 0);
		top_veil[0].position = top_veil[2].position - Vector2f(gap.x / 2, 0);
		top_veil[1].position = top_veil[3].position - Vector2f(gap.x / 2, 0);
		p = pieces[right_big];
		top_veil[4].position = p.getTransform().transformPoint(p.getPoint(5)) + Vector2f(2, -5);
		top_veil[5].position = p.getTransform().transformPoint(p.getPoint(0)) + Vector2f(2, 0);
		top_veil[6].position = top_veil[4].position + Vector2f(gap.x / 2, 0);
		top_veil[7].position = top_veil[5].position + Vector2f(gap.x / 2, 0);
		for (int i = 0; i < 8; i++) {
			bot_veil[i].position = 2.f * middle - top_veil[i].position;
		}
		break;
	default:
		return;
		break;
	}
	for (int i = 0; i < top_veil.getVertexCount(); i++) {
		top_veil[i].color = bg;
		bot_veil[i].color = bg;
		if ((i > top_veil.getVertexCount() - 3) || (i < 2 && top_veil.getVertexCount() == 8)) {
			top_veil[i].color.a = 0;
			bot_veil[i].color.a = 0;
		}
		else {
			top_veil[i].color.a = 255;
			bot_veil[i].color.a = 255;
		}
	}
}

void Intro::setNextAnimation()
{
	switch (active_piece)
	{
	case 0:
		if (pieces[0].getPosition().x < window.getSize().x/2 - pieces[A].getWing().x * 2 + 1) {
			l_move.reset({ gap.x / 2 + pieces[A].getThickness(), 0 }, MoveAnimation::Mode::accelerate);
			r_move.reset({ -(gap.x / 2+ pieces[A_flipped].getThickness()), 0 }, MoveAnimation::Mode::accelerate);
			A_appeared = true;
		}
		else {
			l_move.reset({ gap.x + pieces[I].getThickness(), 0 }, MoveAnimation::Mode::decelerate);
			r_move.reset({ -(gap.x + pieces[I_flipped].getThickness()), 0 }, MoveAnimation::Mode::decelerate);
			active_piece += 2;
		}
		setNextVeil();
		l_animator.playAnimation("l");
		r_animator.playAnimation("r");
		break;
	case 2:
		l_move.reset({ -(gap.x + pieces[left_big].getThickness()), 0 }, MoveAnimation::Mode::settle);
		r_move.reset({ gap.x + pieces[right_big].getThickness(), 0 }, MoveAnimation::Mode::settle);
		active_piece += 2;
		l_parallel_animator.playAnimation("l");
		r_parallel_animator.playAnimation("r");
		setNextVeil();
		l_animator.playAnimation("l");
		r_animator.playAnimation("r");
		wait(seconds(0.2f));
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
		if (active_piece == 0) {
			audio.play(Audio::Sounds::Intro);
		}
	}
}
