.PHONY=clean all
COMPILER=gcc
CFLAGS = -Wall -fsanitize=address -g
all: server
clean:
	- rm -f *.o  server

COMMON =  ./utils/logger.c ./utils/server_utils.c ./utils/tcp_utils.c ./utils/udp_utils.c ./src/parser/parser.c ./src/parser/parser_utils.c ./src/parser/executioner.c ./src/client.c ./src/server_data.c
server:
	$(COMPILER) $(CFLAGS) -o server ./src/server.c $(COMMON)
