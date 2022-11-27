#include <iostream>
#include <SDL2/SDL.h>

#include "./System.h"
#include "./Components.hpp"


class CharacterSetupSystem : public SetupSystem {
    private:
        SDL_Renderer* renderer;

    public:
        CharacterSetupSystem(SDL_Renderer* r) : renderer(r) {}

        ~CharacterSetupSystem() {}

        void run() override {

          Entity player = scene->createEntity(
            "PLAYER",
            200,
            250
          );
          player.addComponent<MovementComponent>(0, 0);
          player.addComponent<SpriteComponent>(0, 2, 15, 0, 0, 50 );
          scene->player = new Entity(player);
        }
};

class EnemySetupSystem : public SetupSystem {
    private:
        SDL_Renderer* renderer;

    public:
        EnemySetupSystem(SDL_Renderer* r) : renderer(r) {}

        ~EnemySetupSystem() {}

        void run() override {

          Entity enemy = scene->createEntity(
            "ENEMY",
            1100,
            650
          );
          enemy.addComponent<MovementComponent>(200, 0);
          enemy.addComponent<SpriteComponent>(1, 2, 25, 0, 0, 55 );
          scene->enemy = new Entity(enemy);
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

bool playedAction = false;
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
          case SDLK_o:
            if (!playedAction){
              playerSprite.y = 2;
              playedAction = true;
            }
            
            break;
          case SDLK_p:
            if (!playedAction){
              playerSprite.y = 3;
              playedAction = true;
            }
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
          
          case SDLK_o:
            playedAction = false;
            break;
          
          case SDLK_p:
            playedAction = false;
            break;


        }
      }

      if (playerMovement.vx < 0) {
        playerSprite.x = 1;
      }
      else if (playerMovement.vx > 0) {
        playerSprite.x = 0;
      }

    }
};

class MovementUpdateSystem : public UpdateSystem {
    public:

        void run(double dT) override {
          const auto view = scene->mRegistry.view<TransformComponent, MovementComponent, TagComponent, SpriteComponent>();
          for (const entt::entity e : view) {
            auto& pos = view.get<TransformComponent>(e);
            auto& vel = view.get<MovementComponent>(e);
            const auto tag = view.get<TagComponent>(e);

            if (tag.tag == "PLAYER"){
              int newPosX = pos.x + vel.vx * dT;
              int newPosy = pos.y + vel.vy * dT;
              if (newPosX > 130 && newPosX < 1200 && newPosy > 245 && newPosy < 890){
                pos.x = newPosX;
                pos.y = newPosy;
              }
            }
            else{

              auto& sprite = view.get<SpriteComponent>(e);
              if (pos.x <= 600)
              {
                vel.vx = 200;
                sprite.x = 0;
              }
              if (pos.x == 700)
              {
                sprite.y = 2;
              }
              if (pos.x == 1000)
              {
                sprite.y = 3;
              }

              if (pos.x >= 1100)
              {
                vel.vx = -200;
                sprite.x = 1;
              }

              pos.x += vel.vx * dT;
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

class SpriteRenderSystem : public SetupSystem, public UpdateSystem, public RenderSystem {
  private:
      SDL_Renderer* renderer;
      SDL_Window* window;
      int FPS;
      std::string name;
      
      const std::string spritesheets[2] = {
          "./assets/bulletA.png",
          "./assets/brock.png",
      };

      SDL_Texture* tilesets[1];
    public:
        SpriteRenderSystem(SDL_Renderer* r, SDL_Window* w, int fps) : renderer(r), window(w), FPS(fps) {}

        ~SpriteRenderSystem() {}

        void run() override {
          for(int i = 0; i < 2; i++) {
            SDL_Surface* surface = IMG_Load(spritesheets[i].c_str());
            tilesets[i] = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
          }
        }

        void run(double dT) override {
          const auto view = UpdateSystem::scene->mRegistry.view<SpriteComponent>();

          Uint32 current = SDL_GetTicks();

          for (const entt::entity e : view) {
            auto& sprite = view.get<SpriteComponent>(e);

            if (sprite.durationSeconds > 0) {
              float animdT = (current - sprite.lastUpdate) / 1000.0f;
              float animFps = sprite.durationSeconds/FPS;
              int framesToUpdate = animdT/animFps;

              if (framesToUpdate > 0) {
                sprite.y += framesToUpdate;
                sprite.y %= sprite.frames;
                sprite.lastUpdate = current;
              }
            }
          }
        }

        void run(SDL_Renderer* renderer) override {
          auto cameraTransform = RenderSystem::scene->mainCamera->getComponent<TransformComponent>();
          auto cameraZoom = RenderSystem::scene->mainCamera->getComponent<CameraComponent>().zoom;
          const int cx = cameraTransform.x;
          const int cy = cameraTransform.y;

          const auto view = RenderSystem::scene->mRegistry.view<TransformComponent, SpriteComponent>();
          for (const entt::entity e : view) {
            const auto pos = view.get<TransformComponent>(e);
            const auto sprite = view.get<SpriteComponent>(e);

            const int dstTileSize = cameraZoom * sprite.size;
            const int spriteX = sprite.x * sprite.size;
            const int spriteY = sprite.y * sprite.size;

            SDL_Rect src = { spriteX, spriteY, sprite.size, sprite.size };
            SDL_Rect dst = { pos.x - cx, pos.y - cy, dstTileSize, dstTileSize };

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderCopy(renderer, tilesets[sprite.sheetIndex], &src, &dst);                        
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
        rect.y += dstTileSize;
      }
    }
};
