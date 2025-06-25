#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "neural_net.hpp" // Incluir o header da rede neural

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int w, int h);
    void handleEvents(); // Ainda pode ser usado para eventos como fechar janela
    void update();
    void render();
    void clean();
    bool running() const;
    float getSurvivalTime() const; // Adiciona método para obter tempo de sobrevivência

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* dinoTexture;
    SDL_Texture* cactusTexture;
    SDL_Texture* backgroundTexture;
    SDL_Texture* groundTexture;

    SDL_Rect dinoRect;
    SDL_Rect cactusRect;
    SDL_Rect backgroundRect;
    SDL_Rect groundRect1;
    SDL_Rect groundRect2;

    bool isRunning;

    // Estado e física do Dinossauro
    float dinoYVelocity;
    bool isJumping;
    bool isDucking; // A IA pode decidir abaixar
    int dinoInitialY;
    int dinoNormalHeight;
    int dinoDuckHeight;
    int screenWidth;
    int screenHeight;

    // Constantes do Jogo
    static const int FEET_ON_GROUND_Y = 340;
    static const int DINO_NORMAL_SPRITE_HEIGHT = 47;
    static const int DINO_DUCK_SPRITE_HEIGHT = 25;
    static const float PLAYER_JUMP_VELOCITY;
    static const float GRAVITY;
    static const int GAME_MOVE_SPEED = 5;
    static const int GROUND_TEXTURE_HEIGHT = 20;

    NeuralNet neuralNet; // Instância da rede neural
    const float JUMP_THRESHOLD = 0.7f; // Limiar para a IA decidir pular
    const float DUCK_THRESHOLD = 0.3f; // Limiar para a IA decidir abaixar

    Uint32 startTime; // Marca o início do jogo
};

#endif
