CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -fsanitize=address -g3
CC = gcc
LDLIBS = -D_GNU_SOURCE


BIN = basic_server
SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDLIBS) $(CFLAGS) -o $@ $^

clean:
	rm -f $(BIN) $(OBJS)

.PHONY: clean
