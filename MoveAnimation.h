#ifndef MoveAnimation_h
#define MoveAnimation_h
#include <SFML/Graphics.hpp>

using namespace sf;
class MoveAnimation {	// custom movement animation for thor::animator
public:
	enum Mode
	{
		uniform, accelerate, decelerate, ac_de, de_ac, settle
	};
	MoveAnimation(Vector2f relative_destination, Mode movement_mode);
	void operator()(Transformable& object, double progress);
	bool is_idle();
private:
	Vector2f source;
	Vector2f destination;
	Mode mode;
	bool firstCall = true;

};

#endif // !MoveAnimation_h