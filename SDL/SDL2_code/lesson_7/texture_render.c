/*
 *  loading an image to render it using textures
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

/* current display texture */
SDL_Texture *current_texture = NULL;


/* starts up SDL and create window */
int init_sdl(void);

/* loads media */
int load_media(void);

/* free media and shutdown SDL */
void close_sdl(void);

/* loads individual image as texture */
SDL_Texture *load_texture(const char *path);





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

			/* clear screen */
			SDL_RenderClear(renderer);

			/* render texture to screen */
			SDL_RenderCopy(renderer, current_texture, NULL, NULL);

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
	int flags = 0;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! "
				"SDL Error: %s\n", SDL_GetError());
		return -1;
	}

	/* set texture filtering to linear */
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		fprintf(stderr,
			"Warning: Linear texture filtering not enabled!\n");


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


	/* initialize PNG loading */
	flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) == 0) {
		fprintf(stderr, "SDL_image could not initialize! "
			"SDL_image Error: %s\n", IMG_GetError());
		return -1;
	}

	return 0;
}



int
load_media(void)
{
	int success = 0;

	current_texture = load_texture("texture.png");
	if (current_texture == NULL) {
		fprintf(stderr, "Failed to load texture image!\n");
		success = -1;
	}

	return success;
}


void
close_sdl(void)
{
	/* free load image */
	SDL_DestroyTexture(current_texture);
	current_texture = NULL;

	/* destroy window */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	/* quit SDL subsystems */
	IMG_Quit();
	SDL_Quit();
}


SDL_Texture *
load_texture(const char *path)
{
	/* the final texture */
	SDL_Texture *texture = NULL;

	/* load image at specified path */
	SDL_Surface *load_surface = IMG_Load(path);


	if (load_surface == NULL)
		fprintf(stderr, "Unable to load image: %s!"
			"SDL_image Error: %s\n", path, IMG_GetError());
	else {
		/* create texture from surface pixels */
		texture = SDL_CreateTextureFromSurface(renderer, load_surface);

		if (texture == NULL)
			fprintf(stderr, "Unable to create texture form %s! "
				"SDL Error: %s\n", path, SDL_GetError());

		/* free old loaded surface */
		SDL_FreeSurface(load_surface);
	}

	return texture;
}




