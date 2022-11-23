#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <string>

struct TagComponent {
  std::string tag;
};

struct TransformComponent {
  int x = 0;
  int y = 0;
};

struct MovementComponent {
  int vx = 0;
  int vy = 0;
};

struct CameraComponent {
  int zoom = 1;
  int vw = 0;
  int vh = 0;
  int ww = 0;
  int wh = 0;
};

struct SpriteComponent {
  int x = 0;
  int y = 0;
  int size = 24;
  SDL_Texture* texture;
};

#endif