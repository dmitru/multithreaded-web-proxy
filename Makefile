
SRC_DIR=./src
INCLUDE_DIR=./include
BIN_DIR=./bin

CC=g++
CC_OPTIONS=-O3 -lpthread -I$(INCLUDE_DIR) -g

all: mkdirs server
	@echo "Done!"

mkdirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(INCLUDE_DIR)

server: $(SRC_DIR)/server.cpp  $(SRC_DIR)/utils.cpp $(SRC_DIR)/request_handler.cpp $(SRC_DIR)/http_utils.cpp
	$(CC) $(CC_OPTIONS) -o $(BIN_DIR)/$@ $^

utils_test: $(SRC_DIR)/utils_test.cpp  $(SRC_DIR)/utils.cpp $(SRC_DIR)/request_handler.cpp $(SRC_DIR)/http_utils.cpp
	$(CC) $(CC_OPTIONS) -o $(BIN_DIR)/$@ $^

clean:
	rm -rf ./bin/*