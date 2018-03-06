CC=gcc
CFLAGS=-Wall -Werror -Wextra
LDFLAGS=-lpthread

EXECUTABLES=clientFTP serveurFTP

all: $(EXECUTABLES)

clientFTP: clientFTP.c readcmd.c csapp.c
	gcc -Wall -g -o $@ $^ -lpthread

serveurFTP: serveurFTP.c readcmd.c csapp.c echo.c
	gcc -Wall -g -o $@ $^ -lpthread

csapp.o: csapp.h

clean:
	rm -rf *.o
	rm -rf $(EXECUTABLES)
