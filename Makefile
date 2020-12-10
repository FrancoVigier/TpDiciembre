CC = gcc
CFLAGS = -g -O3 -lm -Wall -Wextra -Werror -std=c99

.PHONY: default_target all clean

default_target: Interprete
all: default_target

OBJECTS_LSE = $(patsubst %.c, compilados/.obj/%.o, $(wildcard LSE/*.c))
HEADERS_LSE = $(wildcard LSE/*.h)

OBJECTS_HASH = $(patsubst %.c, compilados/.obj/%.o, $(wildcard hash/*.c))
HEADERS_HASH = $(wildcard hash/*.h)

OBJECTS_PARSER = $(patsubst %.c, compilados/.obj/%.o, $(wildcard parser/*.c))
HEADERS_PARSER = $(wildcard parser/*.h)

OBJECTS_INTERPRETE = $(patsubst %.c, compilados/.obj/%.o, $(wildcard *.c))
HEADERS_INTERPRETE = $(wildcard *.h)

compilados/.obj/%.o: %.c $(HEADERS_LSE) $(HEADERS_HASH) $(HEADERS_PARSER) $(HEADERS_INTERPRETE)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: Interprete $(OBJECTS_LSE) $(OBJECTS_HASH) $(OBJECTS_PARSER) $(OBJECTS_INTERPRETE)

Interprete: compilados compilados/.obj compilados/.obj/LSE compilados/.obj/hash compilados/.obj/parser $(OBJECTS_LSE) $(OBJECTS_HASH) $(OBJECTS_PARSER) $(OBJECTS_INTERPRETE)
	$(CC) $(OBJECTS_LSE) $(OBJECTS_HASH) $(OBJECTS_PARSER) $(OBJECTS_INTERPRETE) $(CFLAGS) -o compilados/$@

compilados:
	mkdir -p $@

compilados/.obj/LSE:
	mkdir -p $@

compilados/.obj/hash:
	mkdir -p $@
	
compilados/.obj/parser:
	mkdir -p $@
	
compilados/.obj:
	mkdir -p $@

clean:
	-rm -rf compilados
