/*
 * take a smaller image and stretch it to fit the screen.
 */

#include <stdio.h>
#include <SDL2/SDL.h>



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
	SDL_Rect stretch_rect;


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

			/* apply the image stretch */
			stretch_rect.x = 0;
			stretch_rect.y = 0;
			stretch_rect.w = SCREEN_WIDTH;
			stretch_rect.h = SCREEN_HEIGHT;
			
			SDL_BlitScaled(current_surface, NULL,
					screen_surface, &stretch_rect);

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
	int success = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! "
				"SDL Error: %s\n", SDL_GetError());
		success = -1;
	} else {
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
			success = -1;
		} else
			/* get window surface */
			screen_surface = SDL_GetWindowSurface(window);
	}

	return success;
}


int
load_media(void)
{
	int success = 0;

	current_surface = load_surface("stretch.bmp");
	if (current_surface == NULL) {
		fprintf(stderr, "Failed to load stretch image!\n");
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
	SDL_Quit();
}


SDL_Surface *
load_surface(const char *path)
{
	/* the final otimized image */
	SDL_Surface *optimized_surface = NULL;


	/* load image at specified path */
	SDL_Surface *img_surface = SDL_LoadBMP(path);
	if (img_surface == NULL)
		fprintf(stderr, "Unable to load image: %s!" "SDL Error: %s\n",
			path, SDL_GetError());
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


