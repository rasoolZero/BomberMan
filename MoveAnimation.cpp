#include "MoveAnimation.h"
#include <cmath>

template <class T>
MoveAnimation<T>::MoveAnimation(Vector2f relative_destination, Mode movement_mode)
	:destination{ relative_destination }
	, mode{ movement_mode }
{
}

template <class T>
void MoveAnimation<T>::operator()(T& object, double progress)
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
		break;
	default: //uniform
		modifier = progress;
		break;
	}

	object.setPosition(source + static_cast<float>(modifier) * destination);
}

template <class T>
bool MoveAnimation<T>::is_idle()
{
	return firstCall;
}

template<class T>
void MoveAnimation<T>::reset(Vector2f relative_destination, Mode movement_mode)
{
	this->destination = relative_destination;
	this->mode = movement_mode;
	firstCall = true;
}

template class MoveAnimation<CharShape>;
template class MoveAnimation<Transformable>;
