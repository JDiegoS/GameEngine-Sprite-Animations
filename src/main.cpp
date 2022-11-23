#include <iostream>
#include "Game/game.h"

Game *game = nullptr;

int main(int argc, const char * argv[]){
    game = new Game();

    game->init("MY GAME", 1300, 950);
    game->setup();

    while(game->running()){
        game->frameStart();
        game->handleEvents();
        game->update();
        game->render();
        game->frameEnd();
    }

    game->clean();

    return 0;
}