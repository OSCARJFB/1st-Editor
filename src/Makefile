#	Writen by: Oscar Bergström
#   https://github.com/OSCARJFB
#	
#	MIT License
#	Copyright (c) 2023 Oscar Bergström

cc = gcc
cflags_debug := -g -Wall -Wextra -Werror -pedantic -std=c99

cflags_release := -O3 -march=native -mtune=native -flto -fomit-frame-pointer


main: main.c
	$(cc) main.c allocHandler.c fileHandler.c editorMode.c copy.c $(cflags_debug) -lncurses -o main.o

debug: 
	$(cc) main.c fallocHandler.c fileHandler.c editorMode.c copy.c $(cflags_debug) -g -lncurses -o main.o

release: 
	$(cc) main.c allocHandler.c fileHandler.c editorMode.c copy.c $(cflags_release) -lncurses -o ob

clean:
	rm *.o
