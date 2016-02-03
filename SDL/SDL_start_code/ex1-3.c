/*
 * SDL program: 
 *     - SDL_InitSubSystem()
 *     - SDL_QuitSubSystem()
 
 *
 * Compile: gcc ex1.c -lSDL
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>


int
begin_SDL(Uint32 flags)
{
	if (SDL_WasInit(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL is already running\n");
		return -2;
	}

	if (SDL_Init(flags) == -1) {
		fprintf(stderr, "Unable to init SDL\n");
		return -1;
	} else {
		printf("SDL is init successfully\n");
		return 0;
	}
}

void
end_SDL(void)
{
	if (SDL_WasInit(SDL_INIT_EVERYTHING) == 0) {
		printf("SDL is Not running\n");
	} else {
		SDL_Quit();
		printf("SDL_Quit() successfully\n");
	}

	return;
}


int
main(int argc, char *argv[])
{
	atexit(end_SDL);

	if (begin_SDL(SDL_INIT_VIDEO) == -1)
		exit(EXIT_FAILURE);

	SDL_InitSubSystem(SDL_INIT_AUDIO);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);


	return 0;
}

