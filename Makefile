SRC      := frontend
TESTS    := tests
BIN      := bin
BUILD    := build
FLAGS    := -Wall -std=c++17
LIBS     := -lSDL2 -lSDL2main

.PHONY: all 
all: lynes

.PHONY: dirs
dirs:
	@mkdir -p $(BIN)
	@mkdir -p $(BUILD)

.PHONY: lynes
lynes: dirs $(BIN)/lynes

$(BIN)/lynes: $(SRC)/main.cpp
	$(CXX) $(FLAGS) -I lynes -I /usr/include/SDL2 -mbmi2 -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	@rm -r $(BIN)
	@rm -r $(BUILD)