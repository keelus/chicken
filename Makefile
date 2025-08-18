CC=gcc
CFLAGS=-lz -g
SOURCES= \
	 src/main.c \
	 src/net/client_bound/packet_builder/packet_builder.c \
	 src/net/client_bound/packet_definitions/login.c \
	 src/net/client_bound/packet_definitions/play.c \
	 src/net/server_bound/packet_definitions/handshake.c \
	 src/net/server_bound/packet_definitions/login.c \
	 src/net/server_bound/packet_definitions/play.c \
	 src/net/server_bound/packet_handler.c
BIN_NAME=chicken

build:
	$(CC) $(CFLAGS) -o $(BIN_NAME) $(SOURCES)

run:
	./$(BIN_NAME)

clean:
	rm ./$(BIN_NAME)

generate:
	python3 ./packet_definitions/server_bound/packet_definition_generator.py
	python3 ./packet_definitions/client_bound/packet_definition_generator.py
