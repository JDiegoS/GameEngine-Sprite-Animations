
#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>



class Game {
    public:
        Game();
        ~Game();

        void init(const char* title, int width, int height);
        void setup();
        void handleEvents();
        void update();
        void render();
        void clean();
        bool running();
        void frameStart();
        void frameEnd();
    private:
        bool isRunning;
        SDL_Window *window;
        SDL_Renderer *renderer;
        int counter;
        int window_width;
        int window_height;

        int FPS;
        Uint32 frameStartTimeStamp;
        Uint32 frameEndTimeStamp;
        float frameDuration;
        float dT;
        
};

#endif