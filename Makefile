.PHONY=clean all
COMPILER=gcc
CFLAGS = -Wall -fsanitize=address -g
all: server
clean:
	- rm -f *.o  server

COMMON =  ./utils/logger.c ./utils/tcp_utils.c ./src/parser/parser.c ./src/parser/parser_utils.c ./src/parser/executioner.c ./src/client.c
server:
	$(COMPILER) $(CFLAGS) -o server ./src/server.c $(COMMON)
