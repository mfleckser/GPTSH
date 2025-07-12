CC = gcc
CFLAGS = -Wall -g

main: main.o utils.o subshell.o cJSON.o
	$(CC) -o main main.o utils.o subshell.o cJSON.o

main.o: main.c subshell.h utils.h
	$(CC) $(CFLAGS) -c main.c

utils.o: utils.c utils.h cjson/cJSON.h
	$(CC) $(CFLAGS) -c utils.c

subshell.o: subshell.c subshell.h
	$(CC) $(CFLAGS) -c subshell.c

cJSON.o: cjson/cJSON.c cjson/cJSON.h
	$(CC) $(CFLAGS) -c cjson/cJSON.c

clean:
	rm -f *.o main
