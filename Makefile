CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -fsanitize=address -g3
CC = gcc

BIN = basic_client
SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	./$(BIN)

clean:
	rm -f $(BIN) $(OBJS)

.PHONY: clean
