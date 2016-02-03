/*
 * First SDL program: Press ESC to Quit GUI
 *     - SDL_Init()
 *     - SDL_Quit()
 *
 * Compile: gcc ex1.c -lSDL
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>

void press_ESC_to_quit_v1(void);
void press_ESC_to_quit_v2(void);
void do_some_loop(void);


int
main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_ClearError();
	printf("SDL initialized\n");

	atexit(SDL_Quit);


	/**/
	SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
	printf("Programming is running, press ESC to Quit\n");

	press_ESC_to_quit_v2();
	printf("\n\nGAME OVER\n");

	return 0;
}

void
press_ESC_to_quit_v2(void)
{
	int is_gameover = 0;
	SDL_Event event;
	SDL_Event *e = &event;

	printf("\nIn the press_ESC_to_quit()\n");

	while (is_gameover == 0) {
		SDL_PollEvent(e);

		if (e != NULL) {
			if (e->type == SDL_QUIT)
				is_gameover = 1;

			if (e->type == SDL_KEYDOWN &&
			e->key.keysym.sym == SDLK_ESCAPE) {
				is_gameover = 1;
			}
		}

		do_some_loop();
	}
}


void
press_ESC_to_quit_v1(void)
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
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					is_gameover = 1;
				break;
			default:
				break;
			}
		}
		do_some_loop();
	}
	
}


void
do_some_loop(void)
{
	printf("looping...\n");
}





