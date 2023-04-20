CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes -Wwrite-strings -Waggregate-return -Wunreachable-code -Wconversion
SOURCES = $(wildcard src/*.c)
HEDAERS = $(wildcard include/*.h)
OBJ = ${SOURCES:.c=.o}
IFLAG = -Iinclude/

all: lib

lib: ${OBJ}
	ar rcs ./lib/libdistricomp.a ${OBJ}

%.o: %.c
	${CC} ${IFLAG} ${CFLAGS} -c $< -o $@

clean:
	rm -f ./src/*.o