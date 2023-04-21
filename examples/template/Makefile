CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes -Wwrite-strings -Waggregate-return -Wunreachable-code -Wconversion
LFLAGS = -L../../lib -ldistricomp
IFLAG = -I../../include

all: server client

server: server.c
	${CC} ${CFLAGS} ${LFLAGS} ${IFLAG} server.c -o server.o

client: client.c
	${CC} ${CFLAGS} client.o ${LFLAGS} -o client.o

%.o: %.c
	${CC} -c $< -o $@

clean:
	rm -f *.o