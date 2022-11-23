#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include "../entt/entt.hpp"
#include "Scene.h"

class Entity
{
  public:
    Entity(entt::entity e, Scene* s) {
      std::cout << "Entity Created" << std::endl;

      handle = e;
      scene = s;
    }
    ~Entity() {
      std::cout << "Entity Destroyed" << std::endl;
    }

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
      return scene->mRegistry.emplace<T>(handle, std::forward<Args>(args)...);
    }

    template<typename T>
    void removeComponent() {
      scene->mRegistry.remove<T>(handle);
    }

    template<typename T>
    T& getComponent() {
      return scene->mRegistry.get<T>(handle);
    }
    
    entt::entity handle;

  private:
    Scene* scene;
};

#endif