#include "game.hpp"
#include <iostream>
#include <algorithm> // Para std::max

// Definição das constantes estáticas
const float Game::PLAYER_JUMP_VELOCITY = -18.0f;
const float Game::GRAVITY = 0.8f;

Game::Game() : window(nullptr), renderer(nullptr),
               dinoTexture(nullptr), cactusTexture(nullptr),
               backgroundTexture(nullptr), groundTexture(nullptr),
               dinoRect{}, cactusRect{}, backgroundRect{}, groundRect1{}, groundRect2{},
               isRunning(false),
               dinoYVelocity(0.0f), isJumping(false), isDucking(false),
               dinoInitialY(0), dinoNormalHeight(DINO_NORMAL_SPRITE_HEIGHT), dinoDuckHeight(DINO_DUCK_SPRITE_HEIGHT),
               screenWidth(0), screenHeight(0),
               neuralNet(),
               startTime(0) // Inicializa startTime
               {}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int w, int h) {
    screenWidth = w; // Armazenar dimensões da tela
    screenHeight = h;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Carregar texturas
    SDL_Surface* dinoSurf = IMG_Load("assets/img/dino.png");
    SDL_Surface* cactusSurf = IMG_Load("assets/img/cactus.png");
    SDL_Surface* backgroundSurf = IMG_Load("assets/img/fundo.png"); // Carregar fundo
    SDL_Surface* groundSurf = IMG_Load("assets/img/chao.png");         // Carregar chão

    if (!dinoSurf) std::cerr << "Failed to load dino.png: " << IMG_GetError() << std::endl;
    if (!cactusSurf) std::cerr << "Failed to load cactus.png: " << IMG_GetError() << std::endl;
    if (!backgroundSurf) std::cerr << "Failed to load background.png: " << IMG_GetError() << std::endl;
    if (!groundSurf) std::cerr << "Failed to load ground.png: " << IMG_GetError() << std::endl;

    if (!dinoSurf || !cactusSurf || !backgroundSurf || !groundSurf) {
        if(dinoSurf) SDL_FreeSurface(dinoSurf);
        if(cactusSurf) SDL_FreeSurface(cactusSurf);
        if(backgroundSurf) SDL_FreeSurface(backgroundSurf);
        if(groundSurf) SDL_FreeSurface(groundSurf);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    dinoTexture = SDL_CreateTextureFromSurface(renderer, dinoSurf);
    cactusTexture = SDL_CreateTextureFromSurface(renderer, cactusSurf);
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurf);
    groundTexture = SDL_CreateTextureFromSurface(renderer, groundSurf);

    SDL_FreeSurface(dinoSurf);
    SDL_FreeSurface(cactusSurf);
    SDL_FreeSurface(backgroundSurf);
    SDL_FreeSurface(groundSurf);

    if (!dinoTexture || !cactusTexture || !backgroundTexture || !groundTexture) {
        std::cerr << "Failed to create textures from surfaces: " << SDL_GetError() << std::endl;
        clean(); // clean vai destruir o que foi criado
        return false;
    }

    // Configurar posições e tamanhos iniciais
    dinoNormalHeight = DINO_NORMAL_SPRITE_HEIGHT; // Usar altura do sprite original
    dinoDuckHeight = DINO_DUCK_SPRITE_HEIGHT;   // Altura quando abaixado
    dinoInitialY = FEET_ON_GROUND_Y - dinoNormalHeight;

    dinoRect = {50, dinoInitialY, 44, dinoNormalHeight}; // Largura 44 da imagem original
    // Ajustar Y do cacto para alinhar com o chão
    cactusRect = {screenWidth, FEET_ON_GROUND_Y - 50, 25, 50}; 

    backgroundRect = {0, 0, screenWidth, screenHeight};
    groundRect1 = {0, FEET_ON_GROUND_Y, screenWidth, GROUND_TEXTURE_HEIGHT}; 
    groundRect2 = {screenWidth, FEET_ON_GROUND_Y, screenWidth, GROUND_TEXTURE_HEIGHT};

    startTime = SDL_GetTicks(); // Marca o início do jogo

    isRunning = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
        }
        // Remover ou comentar o input do teclado para pulo e abaixamento
        /*
        if (e.type == SDL_KEYDOWN) {
            if (e.key.repeat == 0) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (!isJumping) {
                            isJumping = true;
                            dinoYVelocity = PLAYER_JUMP_VELOCITY;
                            isDucking = false; // Não pode pular e abaixar ao mesmo tempo
                        }
                        break;
                    case SDLK_DOWN:
                        if (!isJumping) { 
                           isDucking = true;
                        }
                        break;
                }
            }
        }
        if (e.type == SDL_KEYUP) {
             switch (e.key.keysym.sym) {
                case SDLK_DOWN:
                    isDucking = false;
                    break;
            }
        }
        */
    }
}

void Game::update() {
    if (!isRunning) return;

    // Inputs para a rede neural
    // Distância horizontal da frente do dinossauro até a frente do cacto
    // Se o cacto já passou, a distância pode ser grande ou negativa,
    // a rede deve aprender a lidar com isso ou podemos resetar o cacto mais cedo
    // para sempre ter um "próximo" cacto.
    // Por simplicidade, usamos a distância até o cacto atual.
    float distToCactus = static_cast<float>(cactusRect.x - (dinoRect.x + dinoRect.w));
    
    // Normalizar a distância pode ser uma boa ideia, mas vamos usar o valor bruto por enquanto
    // float normalizedDist = distToCactus / screenWidth; 
    // Se o cacto estiver atrás do dinossauro, consideramos a distância como máxima para não pular.
    if (cactusRect.x < dinoRect.x + dinoRect.w) {
         distToCactus = static_cast<float>(screenWidth); // Ou algum valor grande indicando "sem ameaça imediata"
    }


    float gameSpeed = static_cast<float>(GAME_MOVE_SPEED);
    float cactusWidth = static_cast<float>(cactusRect.w);
    // float cactusHeight = static_cast<float>(cactusRect.h); // Poderia ser outro input

    // A rede neural decide a ação
    float nn_output = neuralNet.predict(distToCactus, gameSpeed, cactusWidth);

    // Interpretar a saída da rede neural
    // Resetar isDucking a cada frame, a menos que a NN decida abaixar
    // Isso evita que o dino fique abaixado indefinidamente se a NN parar de mandar o sinal de abaixar
    bool previousDuckingState = isDucking;
    isDucking = false; 

    if (nn_output > JUMP_THRESHOLD) {
        if (!isJumping) {
            isJumping = true;
            dinoYVelocity = PLAYER_JUMP_VELOCITY;
            // isDucking = false; // Garantir que não está abaixado ao pular
        }
    } else if (nn_output < DUCK_THRESHOLD) {
        if (!isJumping) { // Só pode abaixar se não estiver pulando
            isDucking = true;
        }
    }
    // Se não decidiu pular nem abaixar, e estava abaixado, mantém o estado de não abaixado (já feito pelo reset acima)
    // Se estava abaixado e a NN não mandou abaixar mais, ele levanta.


    // Mover cacto
    cactusRect.x -= GAME_MOVE_SPEED;
    if (cactusRect.x + cactusRect.w < 0) {
        cactusRect.x = screenWidth;
        // Opcional: randomizar altura ou tipo do cacto aqui
    }

    // Mover chão (rolagem)
    groundRect1.x -= GAME_MOVE_SPEED;
    groundRect2.x -= GAME_MOVE_SPEED;

    if (groundRect1.x + screenWidth <= 0) { 
        groundRect1.x = groundRect2.x + screenWidth;
    }
    if (groundRect2.x + screenWidth <= 0) { 
        groundRect2.x = groundRect1.x + screenWidth;
    }

    // Lógica de abaixar (controlada pela IA agora)
    int currentDinoEffectiveHeight = dinoNormalHeight;
    if (isDucking && !isJumping) {
        currentDinoEffectiveHeight = dinoDuckHeight;
    }
    dinoRect.h = currentDinoEffectiveHeight;

    // Lógica de Pulo
    int dinoTargetYOnGround = FEET_ON_GROUND_Y - dinoRect.h;

    if (isJumping) {
        dinoRect.y += static_cast<int>(dinoYVelocity);
        dinoYVelocity += GRAVITY;

        if (dinoRect.y >= dinoTargetYOnGround) {
            dinoRect.y = dinoTargetYOnGround;
            isJumping = false;
            dinoYVelocity = 0;
            // Ajustar altura se estava abaixado e aterrissou
            if(isDucking) { // Se a IA ainda quer abaixar ao aterrissar
                dinoRect.h = dinoDuckHeight;
                dinoRect.y = FEET_ON_GROUND_Y - dinoDuckHeight;
            } else {
                dinoRect.h = dinoNormalHeight;
                dinoRect.y = FEET_ON_GROUND_Y - dinoNormalHeight; // Recalcular Y com altura normal
            }
        }
    } else {
        // Se não está pulando, garantir que está na posição correta do chão
        if (isDucking) {
            dinoRect.h = dinoDuckHeight;
            dinoRect.y = FEET_ON_GROUND_Y - dinoDuckHeight;
        } else {
            dinoRect.h = dinoNormalHeight;
            dinoRect.y = FEET_ON_GROUND_Y - dinoNormalHeight;
        }
    }

    // Detecção de colisão
    bool collision =
        dinoRect.x < cactusRect.x + cactusRect.w &&
        dinoRect.x + dinoRect.w > cactusRect.x &&
        dinoRect.y < cactusRect.y + cactusRect.h &&
        dinoRect.h + dinoRect.y > cactusRect.y;

    if (collision) {
        isRunning = false; 
        std::cout << "Game Over - Collision!" << std::endl;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Cor de fundo padrão (branco)
    SDL_RenderClear(renderer);

    // Renderizar fundo
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);
    }

    // Renderizar chão
    if (groundTexture) {
        SDL_RenderCopy(renderer, groundTexture, nullptr, &groundRect1);
        SDL_RenderCopy(renderer, groundTexture, nullptr, &groundRect2);
    }

    // Renderizar dinossauro e cacto
    if (dinoTexture) {
        SDL_RenderCopy(renderer, dinoTexture, nullptr, &dinoRect);
    }
    if (cactusTexture) {
        SDL_RenderCopy(renderer, cactusTexture, nullptr, &cactusRect);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (dinoTexture) SDL_DestroyTexture(dinoTexture);
    if (cactusTexture) SDL_DestroyTexture(cactusTexture);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture); // Limpar nova textura
    if (groundTexture) SDL_DestroyTexture(groundTexture);         // Limpar nova textura

    dinoTexture = nullptr;
    cactusTexture = nullptr;
    backgroundTexture = nullptr;
    groundTexture = nullptr;

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;

    IMG_Quit();
    SDL_Quit();
    isRunning = false; // Garantir que o loop principal pare se clean for chamado
    std::cout << "Game cleaned up." << std::endl;
}

bool Game::running() const {
    return isRunning;
}

float Game::getSurvivalTime() const {
    return (SDL_GetTicks() - startTime) / 1000.0f;
}
