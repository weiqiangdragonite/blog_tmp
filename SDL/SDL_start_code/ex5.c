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


void press_ESC_to_quit(void);
void loop_render(void);



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


SDL_Surface *screen = NULL;
SDL_Surface *front = NULL;
SDL_Surface *back = NULL;

int
main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_ClearError();
	printf("SDL initialized\n");

	atexit(end_SDL);


	/* Creat a SDL window, and get the window's surface. */
	//SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode(640, 480, 32, SDL_FULLSCREEN);
	if (screen == NULL) {
		fprintf(stderr, "SDL_SetVideoMode() failed: %\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* Load a BMP file, and convert it as a surface */
	//SDL_Surface *back = NULL;
	back = SDL_LoadBMP("back.bmp");
	if (back == NULL) {
		fprintf(stderr, "SDL_LoadBMP() failed: %\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* Load a BMP file, and convert it as a surface */
	//SDL_Surface *front = NULL;
	front = SDL_LoadBMP("front.bmp");
	if (front == NULL) {
		fprintf(stderr, "SDL_LoadBMP() failed: %\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	press_ESC_to_quit();

	return 0;
}



void
press_ESC_to_quit(void)
{
	int is_gameover = 0;
	SDL_Event event;

	printf("\nIn the press_ESC_to_quit()\n");

	while (is_gameover == 0) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				printf("SDL_QUIT\n");
				is_gameover = 1;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					is_gameover = 1;
				break;
			default:
				break;
			}
		}
		loop_render();
	}
	
}


void
loop_render(void)
{
	if (SDL_BlitSurface(back, NULL, screen, NULL) != 0)
		return;

	if (SDL_BlitSurface(front, NULL, screen, NULL) != 0)
		return;

	/* Show the SDL window's surface */
	if (SDL_Flip(screen) == -1) {
		fprintf(stderr, "SDL_Flip() failed: %\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}

