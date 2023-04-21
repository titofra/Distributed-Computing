CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes -Wwrite-strings -Waggregate-return -Wunreachable-code -Wconversion
SOURCES = $(wildcard src/*.c)
OBJ = $(subst src/, ,${SOURCES:.c=.o})	# src/example.c -> exemple.o
IFLAG = -Iinclude/

all: lib

lib: obj
	ar rcs ./lib/libdistricomp.a ${OBJ}

obj: ${SOURCES}
	${CC} ${IFLAG} ${CFLAGS} -c ${SOURCES}

#%.o: %.c
#	${CC} ${IFLAG} ${CFLAGS} -c $< -o $@

clean:
	rm -f *.o