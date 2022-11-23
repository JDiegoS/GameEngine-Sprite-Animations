#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "Scene.h"

#include "Entities.hpp"
#include "Components.hpp"
#include "System.h"


Scene::Scene(const std::string& name)
{
  std::cout << "Scene " << name << std::endl;
}

Scene::~Scene()
{
  std::cout << "Scene Destroyed!" << std::endl;
}

Entity Scene::createEntity(const std::string& name, int x, int y)
{
  Entity entity = { mRegistry.create(), this };
  entity.addComponent<TagComponent>(name);
  entity.addComponent<TransformComponent>(x, y);

  return entity;
}

void Scene::addSetupSystem(SetupSystem* s)
{
  s->setScene(this);
  setupSystems.push_back(s);
}

void Scene::addInputSystem(InputSystem* s)
{
  s->setScene(this);
  inputSystems.push_back(s);
}

void Scene::addUpdateSystem(UpdateSystem* s)
{
  s->setScene(this);
  updateSystems.push_back(s);
}

void Scene::addRenderSystem(RenderSystem* s)
{
  s->setScene(this);
  renderSystems.push_back(s);
}

void Scene::setup()
{
  
  for (SetupSystem* sys: setupSystems)
  {
    sys->run();
  }
}

void Scene::update(double dT)
{
  
  for (UpdateSystem* sys: updateSystems)
  {
    sys->run(dT);
  }
}

void Scene::render(SDL_Renderer* renderer)
{
  
  for (RenderSystem* sys: renderSystems)
  {
    sys->run(renderer);
  }
}

void Scene::input(SDL_Event event)
{  
  for (InputSystem* sys: inputSystems)
  {
    sys->run(event);
  }
}