/*
 * open a window and put an image on it, then wait for user exit
 * gcc img_screen.c -lSDL2
 */

#include <stdio.h>
#include <SDL2/SDL.h>


/* screen dimension constrants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/* the window we'll be rendering to */
SDL_Window *window = NULL;

/* the surface contained by the window */
SDL_Surface *screen_surface = NULL;

/* the image we will load and show on the screen */
SDL_Surface *img_surface = NULL;



/* starts up SDL and creates window */
int init_sdl();
/* loads media */
int load_media();
/* frees media and shuts down SDL */
void close_sdl();


int
main(int argc, char *argv[])
{
	/* user quit flag  */
	int quit = 0;
	/* event handler */
	SDL_Event event;

	/* start up SDL and create window */
	if (init_sdl() == -1) {
		fprintf(stderr, "Failed to initialize!\n");
		exit(1);
	}

	/* load media */
	if (load_media("x.bmp") == -1)
		fprintf(stderr, "Failed to load media!\n");
	else {
		/* apply the image */
		SDL_BlitSurface(img_surface, NULL, screen_surface, NULL);

		/* update the surface */
		SDL_UpdateWindowSurface(window);

		/* while application is running */
		while (!quit) {
			/* handle event in queue */
			while (SDL_PollEvent(&event) != 0) {
				/* user requests quit*/
				if (event.type == SDL_QUIT)
					quit = 1;
			}
		}
	}


	/* free resources and close SDL */
	close_sdl();

	return 0;
}



int
init_sdl(void)
{
	/* initialization flag */
	int success = 0;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n",
			SDL_GetError());
		success = -1;
	} else {
		/* create window */
		window = SDL_CreateWindow("SDL Tutorial",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (window == NULL) {
			fprintf(stderr, "Window could not be created!"
				"SDL_Error: %s\n", SDL_GetError());
			success = -1;
		} else {
			/* get window surface */
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return success;
}


int
load_media(const char *img_name)
{
	/* loading success flag */
	int success = 0;

	/* load splash image */
	img_surface = SDL_LoadBMP(img_name);
	if (img_surface == NULL) {
		fprintf(stderr, "Unable to load image %s! SDL_Error:%s\n",
			img_name, SDL_GetError());
		success = -1;
	}

	return success;
}


void
close_sdl(void)
{
	/* deallocate surface */
	SDL_FreeSurface(img_surface);
	img_surface = NULL;

	/* destroy window */
	SDL_DestroyWindow(window);

	/* quit SDL subsystem */
	SDL_Quit();
}










