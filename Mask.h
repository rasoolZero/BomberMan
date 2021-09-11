#ifndef MASK_H
#define MASK_H
#include "MoveAnimation.h"
#include <SFML/Graphics.hpp>
#include <Thor/Animations.hpp>
using namespace sf;
class Mask : public Drawable {
public:
  Mask(Vector2f source, Vector2f destination, Time duration,
       Vector2f wing_vector, Color color);
  void update(Time elapsed);
  Vector2f getPosition(short index = 2);
  float getProgress() { return this->move.getProgress(); }
  void setTailPowerScale(double scale) { this->tail_speed_scale = scale; }
  void setBackTexture(const Texture *back_texture,
                      float back_texture_coefficient = 1.f);
  void setBodyTexture(const Texture *body_texture,
                      float body_texture_coefficient = 1.f);
  void setBodyTextureColor(Color color);
  void setBodyShader(Shader *shader) { this->body_shader = shader; }
  void reset();

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
  void setPosition(Vector2f position);
  Vector2f source;
  Vector2f movement_vector;
  Vector2f wing_vector;
  VertexArray vertices;
  double tail_speed_scale = 1.0;
  VertexArray body; // target for body_texture
  Texture *body_texture = nullptr;
  float body_texture_coefficient; // affects size of texture on target
  VertexArray back;               // target for back_texture
  Texture *back_texture = nullptr;
  float back_texture_coefficient; // affects size of texture on target
  Shader *body_shader;
  // VertexArray glow;
  VertexArray front_glow;
  VertexArray back_glow;
  VertexArray fill;
  VertexArray body_fill;
  Color color;
  MoveAnimation move;
  thor::Animator<Mask, std::string> animator;

  friend class MoveAnimation;
};

#endif // !MASK_H
