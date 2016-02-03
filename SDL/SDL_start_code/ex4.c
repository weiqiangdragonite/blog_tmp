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

int
main(int argc, char *argv[])
{
	putenv("SDL_VIDEODRIVER=dga");

	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_ClearError();
	printf("SDL initialized\n");

	atexit(end_SDL);


	/* Creat a SDL window, and get the window's surface. */
	/* PS: use hardware--> SDL_HWSURFACE */
	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
	if (screen == NULL) {
		fprintf(stderr, "SDL_SetVideoMode() failed: %\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* Show the SDL window's surface */
	if (SDL_Flip(screen) == -1) {
		fprintf(stderr, "SDL_Flip() failed: %\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}


	char name[30];
	if (SDL_VideoDriverName(name, 30) == NULL) {
		fprintf(stderr, "SDL_VideoDriverName() failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	printf("name = %s\n", name);


	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	printf("Is it possible to create hardware surfaces: %d\n",
		info->hw_available);
	printf("Is there a window manager available: %d\n",
		info->wm_available);
	printf("Are hardware to hardware blits accelerated: %d\n",
		info->blit_hw);
	printf("Are hardware to hardware colorkey blits accelerated: %d\n",
		info->blit_hw_CC);
	printf("Are hardware to hardware alpha blits accelerated: %d\n",
		info->blit_hw_A);
	printf("Are software to hardware blits accelerated: %d\n",
		info->blit_sw);
	printf("Are software to hardware colorkey blits accelerated: %d\n",
		info->blit_sw_CC);
	printf("Are software to hardware alpha blits accelerated: %d\n",
		info->blit_sw_A);
	printf("Are color fills accelerated: %d\n", info->blit_fill);
	printf("Total amount of video memory in Kilobytes: %d\n",
		info->video_mem);
	printf("Width of the current video mode: %d\n",
		info->current_w);
	printf("Height of the current video mode: %d\n",
		info->current_h);


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
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					is_gameover = 1;
				break;
			default:
				break;
			}
		}
	}
	
}



