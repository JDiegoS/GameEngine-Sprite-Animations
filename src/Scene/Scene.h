#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "../entt/entt.hpp"

class Entity;
class SetupSystem;
class InputSystem;
class UpdateSystem;
class RenderSystem;

class Scene {
  private:
    std::vector<SetupSystem*> setupSystems;
    std::vector<InputSystem*> inputSystems;
    std::vector<UpdateSystem*> updateSystems;
    std::vector<RenderSystem*> renderSystems;

  public:
    Scene(const std::string&);
    ~Scene();

    entt::registry mRegistry;
    Entity* mainCamera;
    Entity* player;
    Entity* enemy;

    Entity createEntity(
      const std::string& name = "NO NAME",
      int x = 0,
      int y = 0
    );
    void addSetupSystem(SetupSystem* s);
    void addInputSystem(InputSystem* s);
    void addUpdateSystem(UpdateSystem* s);
    void addRenderSystem(RenderSystem* s);

    void setup();
    void update(double dT);
    void input(SDL_Event event);
    void render(SDL_Renderer* renderer);
};

#endif