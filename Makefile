
SRC_DIR=./src
INCLUDE_DIR=./include
BIN_DIR=./bin
LIBS_DIR=./libs

CC=g++
CC_OPTIONS=-O3 -I$(INCLUDE_DIR) -I$(LIBS_DIR)/zlib-1.2.8 -g
LL_OPTIONS=-lpthread -lz
LIBS=$(LIBS_DIR)/zlib-1.2.8/gzlib.o

all: mkdirs server
	@echo "Done!"

zlib:
	@echo "Building zlib library"
	cd $(LIBS_DIR)/zlib-1.2.8 && ./configure && cd -
	make -C $(LIBS_DIR)/zlib-1.2.8/

mkdirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(INCLUDE_DIR)

server: $(SRC_DIR)/server.cpp  $(SRC_DIR)/utils.cpp $(SRC_DIR)/request_handler.cpp $(SRC_DIR)/http_utils.cpp
	$(CC) $(CC_OPTIONS) -o $(BIN_DIR)/$@ $^ $(LIBS) $(LL_OPTIONS)

utils_test: $(SRC_DIR)/utils_test.cpp  $(SRC_DIR)/utils.cpp $(SRC_DIR)/request_handler.cpp $(SRC_DIR)/http_utils.cpp
	$(CC) $(CC_OPTIONS) -o $(BIN_DIR)/$@ $^ $(LL_OPTIONS)


clean:
	rm -rf ./bin/*
	make -C $(LIBS_DIR)/zlib-1.2.8/
