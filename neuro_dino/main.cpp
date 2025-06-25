#include "game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Neuro Dino", 800, 400)) return -1;

    while (game.running()) {
        game.handleEvents();
        game.update();
        game.render();
        SDL_Delay(16); // ~60 FPS
    }

    float survivalTime = game.getSurvivalTime();
    std::cout << "Tempo sobrevivido: " << survivalTime << std::endl;
    game.clean();
    return 0;
}
