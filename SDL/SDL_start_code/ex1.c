/*
 * First SDL program:
 *     - SDL_Init()
 *     - SDL_Quit()
 *
 * Compile: gcc ex1.c -lSDL
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>


int
main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		fprintf(stderr, "Could not initialize SDL\n");
		exit(EXIT_FAILURE);
	}

	printf("SDL initialized\n");
	SDL_Quit();

	return 0;
}

