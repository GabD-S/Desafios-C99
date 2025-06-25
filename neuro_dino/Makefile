CXX = g++
CXXFLAGS = -Wall -Iinclude -std=c++17
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer

# Definir arquivos explicitamente
SOURCES = main.cpp src/game.cpp nn/neural_net.cpp
OBJECTS = main.o src/game.o nn/neural_net.o
BIN = build/neuro_dino

all: $(BIN)

$(BIN): $(OBJECTS)
	mkdir -p build
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

main.o: main.cpp include/game.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

src/game.o: src/game.cpp include/game.hpp include/neural_net.hpp
	$(CXX) $(CXXFLAGS) -c src/game.cpp -o src/game.o

nn/neural_net.o: nn/neural_net.cpp include/neural_net.hpp
	$(CXX) $(CXXFLAGS) -c nn/neural_net.cpp -o nn/neural_net.o

clean:
	rm -f $(OBJECTS) $(BIN)

.PHONY: all clean
