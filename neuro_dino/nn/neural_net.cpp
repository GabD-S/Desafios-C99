#include "neural_net.hpp"
#include <cmath>
#include <fstream>
#include <nlohmann/json.hpp>

NeuralNet::NeuralNet() {
    std::ifstream file("weights.json");
    if (file) {
        nlohmann::json j;
        file >> j;
        w1 = j["w1"].get<std::vector<std::vector<float>>>();
        b1 = j["b1"].get<std::vector<float>>();
        w2 = j["w2"].get<std::vector<float>>();
        b2 = j["b2"].get<float>();
    } else {
        // Pesos e bias padrão caso não exista o arquivo
        w1 = {
            {0.2f, -0.1f, 0.4f},
            {-0.3f, 0.8f, -0.5f},
            {0.7f, -0.2f, 0.1f},
            {-0.6f, 0.3f, 0.9f}
        };

        b1 = {0.1f, -0.2f, 0.05f, 0.3f};

        w2 = {0.5f, -0.4f, 0.6f, -0.3f};
        b2 = 0.2f;
    }
}

float NeuralNet::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

float NeuralNet::predict(float dist, float vel, float width) {
    std::vector<float> input = {dist, vel, width};
    std::vector<float> hidden(4);

    for (int i = 0; i < 4; ++i) {
        float sum = 0.0f;
        for (int j = 0; j < 3; ++j) {
            sum += w1[i][j] * input[j];
        }
        sum += b1[i];
        hidden[i] = sigmoid(sum);
    }

    float out = 0.0f;
    for (int i = 0; i < 4; ++i) {
        out += w2[i] * hidden[i];
    }
    out += b2;
    return sigmoid(out);
}
