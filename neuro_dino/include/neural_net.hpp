#ifndef NEURAL_NET_HPP
#define NEURAL_NET_HPP

#include <vector>
#include <string>

class NeuralNet {
public:
    NeuralNet();

    // Recebe distância, velocidade e largura do cacto
    float predict(float dist, float vel, float width);

private:
    // pesos e bias para uma rede com 3 entradas, 1 camada oculta com 4 neurônios, 1 saída
    std::vector<std::vector<float>> w1;
    std::vector<float> b1;
    std::vector<float> w2;
    float b2;

    float sigmoid(float x);
};

#endif
