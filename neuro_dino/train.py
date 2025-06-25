import numpy as np
import json

# Parâmetros da rede neural
INPUT_SIZE = 3
HIDDEN_SIZE = 4
OUTPUT_SIZE = 1

# Função de ativação
sigmoid = lambda x: 1 / (1 + np.exp(-x))

def random_weights():
    w1 = np.random.uniform(-1, 1, (HIDDEN_SIZE, INPUT_SIZE)).tolist()
    b1 = np.random.uniform(-1, 1, HIDDEN_SIZE).tolist()
    w2 = np.random.uniform(-1, 1, HIDDEN_SIZE).tolist()
    b2 = float(np.random.uniform(-1, 1, 1))
    return {'w1': w1, 'b1': b1, 'w2': w2, 'b2': b2}

def save_weights(weights, filename="weights.json"):
    with open(filename, "w") as f:
        json.dump(weights, f)

def train_dummy():
    # Aqui você implementaria o treinamento real
    # Exemplo: apenas gera pesos aleatórios
    return random_weights()

if __name__ == "__main__":
    for epoch in range(1000):
        weights = train_dummy()
        save_weights(weights)
        print(f"Pesos salvos na iteração {epoch}")
        # Aqui você pode adicionar código para rodar o C++ e avaliar o desempenho
        # Se a IA sobreviver 2 minutos, pare o loop
        # Exemplo: break
        # break
