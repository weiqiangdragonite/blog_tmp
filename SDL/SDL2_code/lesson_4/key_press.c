/*
 * learn to handle keyboard input,  make different images
 * appear depending on which key you press.
 */

#include <stdio.h>
#include <SDL2/SDL.h>


/* key press surfaces constants */
enum key_press_surfaces {
	KEY_PRESS_SURFACE_DEFAULT,	/* == 0 */
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL		/* == 5 */
};

/* screen dimension constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/* the window we will be rendering to */
SDL_Window *window = NULL;

/* the surface contained by the window */
SDL_Surface *screen_surface = NULL;

/* the images correspond to a key press */
SDL_Surface *key_press_surfaces[KEY_PRESS_SURFACE_TOTAL];

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

void select_key_surface(SDL_Event *event);


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

	/* set default current surface */
	current_surface = key_press_surfaces[KEY_PRESS_SURFACE_DEFAULT];

	/* main loop */
	while (!quit) {
		/* handle events on queue */
		while (SDL_PollEvent(&event) != 0) {
			/* user requests quit */
			if (event.type == SDL_QUIT) {
				quit = 1;
				printf("\nBye~\n");
				break;

			/* user presses a key */
			} else if (event.type == SDL_KEYDOWN) {
				select_key_surface(&event);
			}

			/* apply to current image */
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


void
select_key_surface(SDL_Event *event)
{
	/* select surface based on key press */
	switch (event->key.keysym.sym) {
	case SDLK_UP:
		current_surface = key_press_surfaces[KEY_PRESS_SURFACE_UP];
		break;

	case SDLK_DOWN:
		current_surface = key_press_surfaces[KEY_PRESS_SURFACE_DOWN];
		break;

	case SDLK_LEFT:
		current_surface = key_press_surfaces[KEY_PRESS_SURFACE_LEFT];
		break;

	case SDLK_RIGHT:
		current_surface = key_press_surfaces[KEY_PRESS_SURFACE_RIGHT];
		break;

	default:
		current_surface = key_press_surfaces[KEY_PRESS_SURFACE_DEFAULT];
		break;
	}
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

	/* load default surface */
	if ((key_press_surfaces[KEY_PRESS_SURFACE_DEFAULT] =
	load_surface("press.bmp")) == NULL) {
		fprintf(stderr, "Failed to load default image\n");
		success = -1;
	}

	/* load key up surface */
	if ((key_press_surfaces[KEY_PRESS_SURFACE_UP] =
	load_surface("up.bmp")) == NULL) {
		fprintf(stderr, "Failed to load key up image\n");
		success = -1;
	}

	/* load key down surface */
	if ((key_press_surfaces[KEY_PRESS_SURFACE_DOWN] =
	load_surface("down.bmp")) == NULL) {
		fprintf(stderr, "Failed to load key down image\n");
		success = -1;
	}

	/* load key left surface */
	if ((key_press_surfaces[KEY_PRESS_SURFACE_LEFT] =
	load_surface("left.bmp")) == NULL) {
		fprintf(stderr, "Failed to load key left image\n");
		success = -1;
	}

	/* load key right surface */
	if ((key_press_surfaces[KEY_PRESS_SURFACE_RIGHT] =
	load_surface("right.bmp")) == NULL) {
		fprintf(stderr, "Failed to load key right image\n");
		success = -1;
	}

	return success;
}


void
close_sdl(void)
{
	int i;

	/* deallocate key press surfaces */
	for (i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i) {
		SDL_FreeSurface(key_press_surfaces[i]);
		key_press_surfaces[i] = NULL;
	}

	/* destroy window */
	SDL_DestroyWindow(window);
	window = NULL;

	/* quit SDL subsystems */
	SDL_Quit();
}


SDL_Surface *
load_surface(const char *path)
{
	/* load image at specified path */
	SDL_Surface *img_surface = SDL_LoadBMP(path);
	if (img_surface == NULL)
		fprintf(stderr, "Unable to load image: %s!" "SDL Error: %s\n",
			path, SDL_GetError());

	return img_surface;
}


