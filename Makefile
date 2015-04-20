
SRC_DIR=./src
INCLUDE_DIR=./include
BIN_DIR=./bin
LIBS_DIR=./libs

CC=g++
CC_OPTIONS=-O3 -lpthread -lz -I$(INCLUDE_DIR) -I$(LIBS_DIR)/zlib-1.2.8 -g 
LIBS=$(LIBS_DIR)/zlib-1.2.8/gzlib.o

all: zlib mkdirs server
	@echo "Done!"

zlib:
	@echo "Building zlib library"
	cd $(LIBS_DIR)/zlib-1.2.8 && ./configure && cd -
	make -C $(LIBS_DIR)/zlib-1.2.8/

mkdirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(INCLUDE_DIR)

server: $(SRC_DIR)/server.cpp  $(SRC_DIR)/utils.cpp $(SRC_DIR)/request_handler.cpp $(SRC_DIR)/http_utils.cpp
	$(CC) $(CC_OPTIONS) -o $(BIN_DIR)/$@ $^ $(LIBS)

utils_test: $(SRC_DIR)/utils_test.cpp  $(SRC_DIR)/utils.cpp $(SRC_DIR)/request_handler.cpp $(SRC_DIR)/http_utils.cpp
	$(CC) $(CC_OPTIONS) -o $(BIN_DIR)/$@ $^

clean:
	rm -rf ./bin/*
	make -C $(LIBS_DIR)/zlib-1.2.8/
