# Exemplo de script Python para treinar e testar a rede neural do Dino
# Este script gera pesos aleatórios e salva em weights.json
# Você pode substituir a função de treinamento por uma real depois

import numpy as np
import json
import subprocess
import time

INPUT_SIZE = 3
HIDDEN_SIZE = 4
OUTPUT_SIZE = 1

# Função para gerar pesos aleatórios
def random_weights():
    w1 = np.random.uniform(-1, 1, (HIDDEN_SIZE, INPUT_SIZE)).tolist()
    b1 = np.random.uniform(-1, 1, HIDDEN_SIZE).tolist()
    w2 = np.random.uniform(-1, 1, HIDDEN_SIZE).tolist()
    b2 = float(np.random.uniform(-1, 1, 1))
    return {'w1': w1, 'b1': b1, 'w2': w2, 'b2': b2}

# Salva os pesos em um arquivo JSON
def save_weights(weights, filename="weights.json"):
    with open(filename, "w") as f:
        json.dump(weights, f)

def main():
    while True:
        weights = random_weights()
        save_weights(weights)
        print("Pesos salvos. Testando no C++...")
        # Executa o programa C++ e captura o tempo de sobrevivência
        result = subprocess.run(["./build/neuro_dino"], capture_output=True, text=True)
        print(result.stdout)
        # Espera o programa imprimir o tempo de sobrevivência
        for line in result.stdout.splitlines():
            if "Tempo sobrevivido:" in line:
                tempo = float(line.split(":")[-1].strip())
                if tempo >= 120.0:
                    print("A IA sobreviveu por 2 minutos!")
                    return
        time.sleep(1)

if __name__ == "__main__":
    main()
