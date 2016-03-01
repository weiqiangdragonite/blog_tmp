/*
 * Hello SDL2
 *
 * getting a window to pop up
 * gcc hello.c -lSDL2
 */

#include <stdio.h>
#include <SDL2/SDL.h>

/* screen dimension constrants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


int
main(int argc, char *argv[])
{
	/* The window we'll be rendering to */
	SDL_Window *window = NULL;

	/* The surface contained by the window */
	SDL_Surface *screen_surface = NULL;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n",
			SDL_GetError());
	else {
		/* create window */
		window = SDL_CreateWindow("SDL Tutorial",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				SCREEN_WIDTH, SCREEN_HEIGHT,
				SDL_WINDOW_SHOWN);
		if (window == NULL)
			fprintf(stderr, "Window could not be created! "
				"SDL_Error: %s\n", SDL_GetError());
		else {
			/* create window surface */
			screen_surface = SDL_GetWindowSurface(window);

			/* fill the surface white */
			SDL_FillRect(screen_surface, NULL, SDL_MapRGB(
				screen_surface->format, 0xFF, 0xFF, 0xFF));

			/* update the surface */
			SDL_UpdateWindowSurface(window);

			/* wait two seconds */
			SDL_Delay(2000);
		}
	}

	/* Destroy window */
	SDL_DestroyWindow(window);

	/* quit SDL subsystems */
	SDL_Quit();

	return 0;
}

















