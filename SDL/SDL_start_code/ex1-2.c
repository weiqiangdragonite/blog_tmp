/*
 * SDL program: 
 *     - SDL_Init()
 *     - SDL_WasInit()
 *     - SDL_Quit()
 
 *
 * Compile: gcc ex1.c -lSDL
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>


void
beginSDL(Uint32 flags)
{
	if (SDL_WasInit(SDL_INIT_EVERYTHING) != 0)
		printf("SDL is already running\n");

	if (SDL_Init(flags) == -1)
		fprintf(stderr, "Unable to init SDL\n");
	else
		printf("SDL is running successfully\n");

	return;
}


int
main(int argc, char *argv[])
{
	Uint32 subsystem_init = 0;
	Uint32 subsystem_mask = 0;

	if (SDL_Init(SDL_INIT_AUDIO) == -1) {
		fprintf(stderr, "Could not initialize SDL\n");
		exit(EXIT_FAILURE);
	}

	printf("SDL initialized\n");

	subsystem_init = SDL_WasInit(SDL_INIT_EVERYTHING);

	/* check for specific subsystem */
	if (subsystem_init & SDL_INIT_VIDEO)
		printf("Video is initialized\n");
	else
		printf("Video is not initialized\n");

	if (subsystem_init & SDL_INIT_AUDIO)
		printf("Audio is initialized\n");
	else
		printf("Audio is not initialized\n");

	/* check for two subsystems */
	subsystem_mask = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
	if (SDL_WasInit(subsystem_mask) == subsystem_mask)
		printf("Video and Audio initialized\n");
	else
		printf("Video and Audio not initialized\n");

	SDL_Quit();

	return 0;
}

