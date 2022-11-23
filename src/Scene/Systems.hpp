#include <iostream>
#include <SDL2/SDL.h>

#include "./System.h"
#include "./Components.hpp"


class CharacterSetupSystem : public SetupSystem {
    private:
        const std::string spritefile = "assets/bullet.png";        
        SDL_Renderer* renderer;

    public:
        CharacterSetupSystem(SDL_Renderer* r) : renderer(r) {}

        ~CharacterSetupSystem() {}

        void run() override {
          auto cameraComponent = scene->mainCamera->getComponent<CameraComponent>();

          SDL_Surface* surface = IMG_Load(spritefile.c_str());
          SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
          SDL_FreeSurface(surface);

          Entity player = scene->createEntity(
            "PLAYER",
            200,
            250
          );
          player.addComponent<MovementComponent>(0, 0);
          player.addComponent<SpriteComponent>(0, 0, 50, texture);
          scene->player = new Entity(player);
        }
};


class CameraSetupSystem : public SetupSystem {
    private:
      int viewportWidth;
      int viewportHeight;
      int worldWidth;
      int worldHeight;
      int zoom;

    public:
        CameraSetupSystem(int z, int vw, int vh, int ww, int wh) : 
          zoom(z),
          viewportWidth(vw),
          viewportHeight(vh),
          worldWidth(ww),
          worldHeight(wh)
        {}

        void run() override {
          Entity camera = scene->createEntity("CAMERA",
            0,
            0 
          );
          camera.addComponent<CameraComponent>(
            zoom,
            viewportWidth,
            viewportHeight,
            worldWidth,
            worldHeight
          );
          scene->mainCamera = new Entity(camera);
        }
};

class PlayerInputSystem : public InputSystem {
  public:
    void run(SDL_Event event) override {
      auto& playerMovement = scene->player->getComponent<MovementComponent>();
      auto& playerSprite = scene->player->getComponent<SpriteComponent>();

      int speed = 400;

      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym) {
          case SDLK_a:
            playerMovement.vx = -speed;
            break;
          case SDLK_d:
            playerMovement.vx = speed;
            break;
          case SDLK_w:
            playerMovement.vy = -speed;
            break;
          case SDLK_s:
            playerMovement.vy = speed;
            break;
        }
      }  
      if (event.type == SDL_KEYUP)
      {
        switch (event.key.keysym.sym) {
          case SDLK_a:
            playerMovement.vx = 0;

            break;
          case SDLK_d:
            playerMovement.vx = 0;

            break;
          case SDLK_w:
            playerMovement.vy = 0;

          case SDLK_s:
            playerMovement.vy = 0;

        }
      }

    }
};

class MovementUpdateSystem : public UpdateSystem {
    public:

        void run(double dT) override {
          const auto view = scene->mRegistry.view<TransformComponent, MovementComponent>();
          for (const entt::entity e : view) {
            auto& pos = view.get<TransformComponent>(e);
            const auto vel = view.get<MovementComponent>(e);

            int newPosX = pos.x + vel.vx * dT;
            int newPosy = pos.y + vel.vy * dT;
            if (newPosX > 130 && newPosX < 1200 && newPosy > 245 && newPosy < 890){
              pos.x = newPosX;
              pos.y = newPosy;
            }

          }
        }
};

class CameraFollowUpdateSystem : public UpdateSystem {
    public:
        void run(double dT) override {
          auto playerTransform = scene->player->getComponent<TransformComponent>();
          auto cameraComponent = scene->mainCamera->getComponent<CameraComponent>();
          auto& cameraTransform = scene->mainCamera->getComponent<TransformComponent>();

          int px = playerTransform.x - cameraComponent.vw / 2 + 12 * cameraComponent.zoom;
          int py = playerTransform.y - cameraComponent.vh / 2 + 12 * cameraComponent.zoom;

          if (px > 0 && px < cameraComponent.ww - cameraComponent.vw) {
            cameraTransform.x = playerTransform.x - cameraComponent.vw / 2 + 12 * cameraComponent.zoom;
          }

          if (py > 0 && py < cameraComponent.wh - cameraComponent.vh) {
            cameraTransform.y = playerTransform.y - cameraComponent.vh / 2 + 12 * cameraComponent.zoom;
          }
        }
};

class SpriteRenderSystem : public RenderSystem {
    public:
        SpriteRenderSystem() {}

        ~SpriteRenderSystem() {}

        void run(SDL_Renderer* renderer) override {
          auto cameraTransform = scene->mainCamera->getComponent<TransformComponent>();
          auto cameraZoom = scene->mainCamera->getComponent<CameraComponent>().zoom;
          const int cx = cameraTransform.x;
          const int cy = cameraTransform.y;

          const auto view = scene->mRegistry.view<TransformComponent, SpriteComponent>();
          for (const entt::entity e : view) {
            const auto pos = view.get<TransformComponent>(e);
            const auto sprite = view.get<SpriteComponent>(e);
            const int dstTileSize = cameraZoom * sprite.size;

            SDL_Rect src = { sprite.x, sprite.y, sprite.size, sprite.size };
            SDL_Rect dst = { pos.x - cx, pos.y - cy, dstTileSize, dstTileSize };

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderCopy(renderer, sprite.texture, &src, &dst);                        
          }
        }
};

class KnockDownPointSystem : public InputSystem {
  private:
    int ko1 = 0;
    int ko2 = 0;

  public:

    void run(SDL_Event event) override {
        if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym) {
          case SDLK_o:
            // Logica para revisar hizo land el punch
            point1();
            break;
          case SDLK_p:
            // Logica para revisar si recivio punch
            point2();
            break;
        }
      }  
    }

    void point1 (){
      ko1 ++;
      std::cout << "POINTS" << std::endl;
      std::cout << ko1 << " - " << ko2 << std::endl;
    }
    void point2 (){
      ko2 ++;
      std::cout << "POINTS" << std::endl;
      std::cout << ko1 << " - " << ko2 << std::endl;
    }
};

class TileMapSystem : public SetupSystem, public RenderSystem {
  private:
    SDL_Renderer* renderer;
    SDL_Window* window;

    constexpr static int x = 0;
    constexpr static int y = 0;
    constexpr static int tileWidth = 60;
    constexpr static int tileHeigth = 60;

    const std::string mmap = "assets/map.png";
    const std::string file = "assets/tiles.png";
    int tilesWidth;
    int tilesHeight;

    SDL_Rect* tilemap;

    SDL_Texture* texture;
    

  public:
    TileMapSystem(SDL_Renderer* r, SDL_Window* w) : renderer(r), window(w) {
      std::cout << "Tile map system started" << std::endl;
    }

    ~TileMapSystem() {
    }

    // setup
    void run() override {

      SDL_Surface* surface = IMG_Load(file.c_str());
      texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FreeSurface(surface);
  
      STexture* t = new STexture(renderer, window);
      t->load(mmap);
      tilesWidth = t->getWidth();
      tilesHeight = t->getHeight();
      const int totalTiles = tilesWidth * tilesHeight;

      tilemap = new SDL_Rect[totalTiles];

      for(int i = 0; i < totalTiles; i++) {
        Uint32 currentColor = t->getPixel(i);
        int r = ((int)(currentColor >> 16) & 0xff);
        int g = ((int)(currentColor >> 8) & 0xff);
        tilemap[i] = { r * 16, g * 16, 16, 16 };
      }
      
      delete t;
    }

    void run(SDL_Renderer* r) override {

      auto cameraTransform = RenderSystem::scene->mainCamera->getComponent<TransformComponent>();
      auto cameraZoom = RenderSystem::scene->mainCamera->getComponent<CameraComponent>().zoom;
      const int dstTileSize = cameraZoom * 16;
      const int cx = cameraTransform.x;
      const int cy = cameraTransform.y;

      SDL_Rect rect = { -cx, -cy, dstTileSize, dstTileSize };
      for(int i = 0; i < tilesHeight; i++) {
        for(int j = 0; j < tilesWidth; j++) {
          SDL_Rect src = tilemap[i*tilesWidth + j];
          SDL_RenderCopy(r, texture, &src, &rect);
          rect.x += tileWidth;
        }
        rect.x = -cx;
        rect.y += tileHeigth;
      }
    }
};
