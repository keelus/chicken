CC=gcc
CFLAGS=-g
SOURCES= \
	 src/main.c
BIN_NAME=chicken

build:
	$(CC) $(CFLAGS) -o $(BIN_NAME) $(SOURCES)

run:
	./$(BIN_NAME)

clean:
	rm ./$(BIN_NAME)
