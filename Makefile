
SRC_DIR=./src
INCLUDE_DIR=./include
BIN_DIR=./bin

CC=g++
CC_OPTIONS=-O3 -lpthread -I$(INCLUDE_DIR) -std=c++11

all: mkdirs server
	@echo "Done!"

mkdirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(INCLUDE_DIR)

server: $(SRC_DIR)/server.cpp 
	$(CC) $(CC_OPTIONS) -o $(BIN_DIR)/$@ $^

clean:
	rm -rf ./bin/*