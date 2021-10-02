.PHONY : all clean test cleanTest server
COMPILER=gcc
CFLAGS = -Wall -fsanitize=address -g -pedantic
all: server
clean:
	- rm -f *.o  server

COMMON =  ./utils/logger.c ./utils/server_utils.c ./utils/tcp_utils.c ./utils/udp_utils.c ./src/parser/parser.c ./src/parser/parser_utils.c ./src/parser/executioner.c ./src/client.c ./src/server_data.c
server:
	$(COMPILER) $(CFLAGS) -o server ./src/server.c $(COMMON)

test: clean all
	mkdir tests; valgrind --leak-check=full -v ./server 2>> tests/results.valgrind; cppcheck --quiet --enable=all --force --inconclusive src/server.c 2>> tests/output.cppOut

cleanTest:
	rm -rf tests/output.cppOut tests/report.tasks tests/results.valgrind tests

