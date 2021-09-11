#ifndef MOVEANIMATION_h
#define MOVEANIMATION_h
#include <SFML/Graphics.hpp>
using namespace sf;

class MoveAnimation { // custom movement animation for thor::animator
public:
  enum Mode { uniform, accelerate, decelerate, ac_de, de_ac, settle };
  MoveAnimation(Vector2f relative_destination, Mode movement_mode);
  template <class T> void operator()(T &object, double progress);
  bool is_idle();
  double getProgress() const { return this->progress; }
  void reset() {
    firstCall = true;
    progress = 0.0;
  }
  void reset(Vector2f relative_destination, Mode movement_mode);
  void setDestination(Vector2f relative_destination) {
    this->destination = relative_destination;
  }
  void setMode(Mode mode) { this->mode = mode; }

private:
  Vector2f source;
  Vector2f destination;
  Mode mode;
  bool firstCall = true;
  double progress = 0.0;
};

#endif // !MOVEANIMATION_h
