#include "MoveAnimation.h"
#include <cmath>

MoveAnimation::MoveAnimation(Vector2f relative_destination, Mode movement_mode)
	:destination{ relative_destination }
	, mode{ movement_mode }
{
}

template <class T>
void MoveAnimation::operator()(T& object, double progress)
{
	if (firstCall) {
		source = object.getPosition();
		firstCall = false;
	}
	//prepare "firstCall" for next animation if the current one is complete:
	//firstCall = progress == 1.0f; //irrelevant in thor2.0(progress is not guaranteed to be 1 on last call), pending thor2.1

	double modifier;

	switch (mode)
	{
	case Mode::accelerate :
		modifier = pow(progress, 2);
		break;
	case Mode::decelerate :
		modifier = cbrt(progress);
		break;
	case Mode::ac_de :
		modifier = (cbrt(2 * progress - 1) + 1) / 2;
		break;
	case Mode::de_ac :
		modifier = (pow(2 * progress - 1, 3) + 1) / 2;
		break;
	case Mode::settle :
		modifier = -1 * abs(1.25 * progress - 1.125) + 1.125;
		break;
	default: //uniform
		modifier = progress;
		break;
	}

	object.setPosition(source + static_cast<float>(modifier) * destination);
}

bool MoveAnimation::is_idle()
{
	return firstCall;
}

void MoveAnimation::reset(Vector2f relative_destination, Mode movement_mode)
{
	this->destination = relative_destination;
	this->mode = movement_mode;
	firstCall = true;
}

template void MoveAnimation::operator() < CharShape >(CharShape&, double);
template void MoveAnimation::operator() < Transformable > (Transformable&, double);
