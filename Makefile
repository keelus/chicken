CC=gcc
CFLAGS=-std=c23 -Wextra -Wall -pedantic -g 
LDFLAGS=-lz
SOURCES= \
	 src/main.c \
	 src/net/client_bound/packet_builder/packet_builder.c \
	 src/net/client_bound/packet_definitions/login.c \
	 src/net/client_bound/packet_definitions/play.c \
	 src/net/server_bound/packet_definitions/handshake.c \
	 src/net/server_bound/packet_definitions/login.c \
	 src/net/server_bound/packet_definitions/play.c \
	 src/net/server_bound/packet_handler.c \
	 src/net/server_bound/raw_packet.c \
	 src/net/server_bound/fields/string.c \
	 src/net/server_bound/fields/unsigned_short.c \
	 src/net/server_bound/fields/varint.c \
	 src/net/client/client.c
BIN_NAME=chicken

build:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_NAME) $(SOURCES)

run:
	./$(BIN_NAME)

clean:
	rm ./$(BIN_NAME)

generate:
	python3 ./packet_definitions/server_bound/packet_definition_generator.py
	python3 ./packet_definitions/client_bound/packet_definition_generator.py
