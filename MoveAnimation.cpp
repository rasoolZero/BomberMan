#include "MoveAnimation.h"
#include <cmath>
MoveAnimation::MoveAnimation(Vector2f relative_destination, Mode movement_mode)
	:destination{ relative_destination }
	, mode{ movement_mode }
{
}

void MoveAnimation::operator()(Transformable& object, double progress)
{
	if (firstCall) {
		source = object.getPosition();
	}
	//prepare "firstCall" for next animation if the current one is complete:
	firstCall = progress == 1.0f;

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

