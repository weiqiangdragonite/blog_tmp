/*
 * take a smaller image and stretch it to fit the screen.
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>



/* screen dimension constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/* the window we will be rendering to */
SDL_Window *window = NULL;

/* the surface contained by the window */
SDL_Surface *screen_surface = NULL;

/* current display image surface */
SDL_Surface *current_surface = NULL;


/* starts up SDL and create window */
int init_sdl(void);

/* loads media */
int load_media(void);

/* free media and shutdown SDL */
void close_sdl(void);

/* loads individual image */
SDL_Surface *load_surface(const char *path);




int
main(int argc, char *argv[])
{
	int quit = 0;
	SDL_Event event;


	/* start up SDL and create window */
	if (init_sdl() == -1) {
		fprintf(stderr, "Failed to initialize!\n");
		exit(1);
	}

	/* load media */
	if (load_media() == -1) {
		fprintf(stderr, "Failed to load media!\n");
		exit(2);
	}


	/* main loop */
	while (!quit) {
		/* handle events on queue */
		while (SDL_PollEvent(&event) != 0) {
			/* user requests quit */
			if (event.type == SDL_QUIT) {
				quit = 1;
				printf("\nBye~\n");
				break;
			}

			/* apply the PNG image */
			SDL_BlitSurface(current_surface, NULL,
					screen_surface, NULL);

			/* update the surface */
			SDL_UpdateWindowSurface(window);
		}
	}

	/* free resources and close sdl */
	close_sdl();

	return 0;
}



int
init_sdl(void)
{
	int flags = 0;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! "
				"SDL Error: %s\n", SDL_GetError());
		return -1;
	}


	/* create window */
	window = SDL_CreateWindow(
			"SDL Tutorial",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);

	if (window == NULL) {
		fprintf(stderr, "Window could not be created!"
				"SDL Error: %s\n", SDL_GetError());
		return -1;
	}


	/* initialize PNG loading */
	flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) == 0) {
		fprintf(stderr, "SDL_image could not initialize! "
			"SDL_image Error: %s\n", IMG_GetError());
		return -1;
	} else {
		/* get window surface */
		screen_surface = SDL_GetWindowSurface(window);
	}

	return 0;
}


int
load_media(void)
{
	int success = 0;

	current_surface = load_surface("loaded.png");
	if (current_surface == NULL) {
		fprintf(stderr, "Failed to load PNG image!\n");
		success = -1;
	}

	return success;
}


void
close_sdl(void)
{
	/* free load image */
	SDL_FreeSurface(current_surface);
	current_surface = NULL;

	/* destroy window */
	SDL_DestroyWindow(window);
	window = NULL;
	

	/* quit SDL subsystems */
	IMG_Quit();
	SDL_Quit();
}


SDL_Surface *
load_surface(const char *path)
{
	/* the final otimized image */
	SDL_Surface *optimized_surface = NULL;


	/* load image at specified path */
	SDL_Surface *img_surface = IMG_Load(path);

	if (img_surface == NULL)
		fprintf(stderr, "Unable to load image: %s!"
			"SDL_image Error: %s\n", path, IMG_GetError());
	else {
		/* convert surface to screen format */
		optimized_surface = SDL_ConvertSurface(
			img_surface, screen_surface->format, 0);

		if (optimized_surface == NULL)
			fprintf(stderr, "Unable to otimized image: %s! "
				"SDL Error: %s\n", path, SDL_GetError());

		/* free old loaded surface */
		SDL_FreeSurface(img_surface);
	}

	return optimized_surface;
}


