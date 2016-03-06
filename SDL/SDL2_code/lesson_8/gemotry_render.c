/*
 * render some common shapes using primitive rendering
 *
 * gcc xxx -lSDL2 -l SDL2_image
 */


#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


/* screen dimension constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/* the window we will be rendering to */
SDL_Window *window = NULL;

/* window renderer */
SDL_Renderer *renderer = NULL;



/* starts up SDL and create window */
int init_sdl(void);

/* loads media */
int load_media(void);

/* free media and shutdown SDL */
void close_sdl(void);






int
main(int argc, char *argv[])
{
	int quit = 0;
	int i;
	SDL_Event event;


	/* start up SDL and create window */
	if (init_sdl() == -1) {
		fprintf(stderr, "Failed to initialize!\n");
		exit(1);
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

			/* clear screen */
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(renderer);


			/* render red filled quad */
			SDL_Rect fill_rect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4,
					SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
			SDL_RenderFillRect(renderer, &fill_rect);

			/* render green outlined quad */
			SDL_Rect outline_rect = {SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6,
					SCREEN_WIDTH * 2/3.0,
					SCREEN_HEIGHT * 2.0/3.0};
			SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
			SDL_RenderDrawRect(renderer, &outline_rect);

			/* draw blue horizontal line */
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
			SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT / 2,
						SCREEN_WIDTH, SCREEN_HEIGHT / 2);

			/* draw vertical line of yellow dots */
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
			for (i = 0; i < SCREEN_HEIGHT; i += 4)
				SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, i);


			/* update screen */
			SDL_RenderPresent(renderer);
		}
	}

	/* free resources and close sdl */
	close_sdl();

	return 0;
}







int
init_sdl(void)
{
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

	/* create renderer for window */
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		fprintf(stderr, "Renderer could not be created! "
			"SDL Error: %s\n", SDL_GetError());
		return -1;
	}

	/* initialize renderer color */
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	return 0;
}



int
load_media(void)
{
	int success = 0;

	return success;
}


void
close_sdl(void)
{
	/* destroy window */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	/* quit SDL subsystems */
	SDL_Quit();
}


