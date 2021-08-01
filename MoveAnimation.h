#ifndef MOVEANIMATION_h
#define MOVEANIMATION_h
#include <SFML/Graphics.hpp>
#include "CharShape.h"
using namespace sf;

template <class T>
class MoveAnimation {	// custom movement animation for thor::animator
public:
	enum Mode
	{
		uniform, accelerate, decelerate, ac_de, de_ac, settle
	};
	MoveAnimation(Vector2f relative_destination, Mode movement_mode);
	void operator()(T& object, double progress);
	bool is_idle();
	void reset() { firstCall = true; }
	void reset(Vector2f relative_destination, Mode movement_mode);
	void setDestination(Vector2f relative_destination) { this->destination = relative_destination; }
	void setMode(Mode mode) { this->mode = mode; }
private:
	Vector2f source;
	Vector2f destination;
	Mode mode;
	bool firstCall = true;

};


#endif // !MOVEANIMATION_h