/*
 * -lSDL2 -lSDL2_image -lSDL2_mixer
 */


#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL_ttf.h>



/* screen dimension constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


/* testure wrapper class */
class LTexture
{
public:
	/* initializes variables */
	LTexture();

	/* dealloates memory */
	~LTexture();

	/* loads image at specified path */
	int loadFromFile(const char *path);

#ifdef _SDL_TTF_H
	/* created image from font string */
	int loadFromRenderedText(const char *text, SDL_Color textColor);
#endif
	/* deallocates texture */
	void free(void);

	/* set color modulation */
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	/* set blending */
	void setBlendMode(SDL_BlendMode blending);

	/* set alpha modulation */
	void setAlpha(Uint8 alpha);

	/* renders texture at given point */
	void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0,
		SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	/* gets image dimensions */
	int getWidth(void);
	int getHeight(void);

private:
	/* the actual hardware texture */
	SDL_Texture *mTexture;

	/* image dimensions */
	int mWidth;
	int mHeight;
};







/* the window we'll be rendering to */
SDL_Window *gWindow = NULL;

/* the window renderer */
SDL_Renderer *gRenderer = NULL;

/* scence textures */
LTexture gPromptTexture;


/* the music that will be played */
Mix_Music *gMusic = NULL;

/* the sound effects that will be used */
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;





LTexture::LTexture()
{
	/* initialize */
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	printf("LTexture::~LTexture()\n");
	/* deallocate */
	free();
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	/* modulate texture */
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	/* set blending function */
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	/* modulate texture alpha */
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

int LTexture::loadFromFile(const char *path)
{
	/* get rid of preexisting trexture */
	free();

	/* the final texture */
	SDL_Texture *newTexture = NULL;

	/* load image at specified path */
	SDL_Surface *loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
		fprintf(stderr, "Unable to load image %s! "
			"SDL_image Error: %s\n", path, IMG_GetError());
	else {
		/* color key image */
		SDL_SetColorKey(loadedSurface, SDL_TRUE,
			SDL_MapRGB(loadedSurface->format, 0x00, 0xFF, 0xFF));

		/* create texture from surface pixels */
		newTexture = SDL_CreateTextureFromSurface(
				gRenderer, loadedSurface);

		if (newTexture == NULL)
			fprintf(stderr, "Unable to create texture form %s! "
				"SDL Error: %s\n", path, SDL_GetError());
		else {
			/* get image dimensions */
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		/* get rid of old loaded surface */
		SDL_FreeSurface(loadedSurface);
	}

	/* return success */
	mTexture = newTexture;
	return (mTexture == NULL ? -1 : 0);
}

#ifdef _SDL_TTF_H
int LTexture::loadFromRenderedText(const char *text, SDL_Color textColor)
{
	/* get rid of preexisting trexture */
	free();

	/* render text surface */
	SDL_Surface *textSurface =
		TTF_RenderText_Solid(gFont, text, textColor);

	if (textSurface == NULL)
		fprintf(stderr, "Unable to render text surface! "
			"SDL_ttf Error: %s\n", TTF_GetError());
	else {
		/* create texture from surface pixels */
		mTexture = SDL_CreateTextureFromSurface(
				gRenderer, textSurface);

		if (mTexture == NULL)
			fprintf(stderr,
				"Unable to create texture form rendered text! "
				"SDL Error: %s\n", SDL_GetError());
		else {
			/* get image dimensions */
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		/* get rid of old loaded surface */
		SDL_FreeSurface(textSurface);
	}

	/* return success */
	return (mTexture == NULL ? -1 : 0);
}
#endif


void LTexture::free(void)
{
	printf("LTexture::free()\n");
	/* free texture if it exists */
	if (mTexture != NULL) {
		printf("free() ~ width = %d, height = %d\n", mWidth, mHeight);
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;

		mWidth = 0;
		mHeight = 0;
	}
}


void LTexture::render(int x, int y, SDL_Rect *clip, double angle,
	SDL_Point *center, SDL_RendererFlip flip)
{
	/* set rendering space and render to screen */
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};

	/* set clip rendering dimensions */
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	/* render to screen */
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad,
		angle, center, flip);
}

int LTexture::getWidth(void)
{
	return mWidth;
}

int LTexture::getHeight(void)
{
	return mHeight;
}









/* starts up SDL and creates window */
int init_sdl(void);

/* loads medis */
int load_media(void);

/* frees media and shuts down SDL */
void close_sdl(void);




int
main(int argc, char *argv[])
{
	int quit = 0;
	/* event handler */
	SDL_Event event;


	/* starts up SDL and create window */
	if (init_sdl() == -1) {
		fprintf(stderr, "Failed to initialize SDL!\n");
		return -1;
	}

	/* load media */
	if (load_media() == -1) {
		fprintf(stderr, "Failed to load media!\n");
		return -2;
	}

	/* main loop */
	while (quit == 0) {
		/* handle events on queue */
		while (SDL_PollEvent(&event) != 0) {
			/* users request quit */
			if (event.type == SDL_QUIT)
				quit = 1;

			/* handle key press */
			/* 好像数字键盘的数字按键不行，只能是字母那边的数字按键才行 */
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_1:	/* play high sound effect */
					Mix_PlayChannel(-1, gHigh, 0);
					break;
				case SDLK_2:	/* play medium sound effect */
					Mix_PlayChannel(-1, gMedium, 0);
					break;

				case SDLK_3:	/* play low sound effect */
					Mix_PlayChannel(-1, gLow, 0);
					break;
				case SDLK_4:	/* play scratc sound effect */
					Mix_PlayChannel(-1, gScratch, 0);
					break;

				case SDLK_9:
					/* if there is no music playing */
					/* play the music */
					if (Mix_PlayingMusic() == 0)
						Mix_PlayMusic(gMusic, -1);

					/* else if music is being played */
					else  {
						/* if the music is paused */
						/* resume the music */
						if (Mix_PausedMusic() == 1)
							Mix_ResumeMusic();

						/* if the music is playing */
						/* pause the music */
						else
							Mix_PauseMusic();
					}
					break;

				case SDLK_0:	/* stop the music */
					Mix_HaltMusic();
					break;
				}
			}
		}

		/* clear screen */
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		/* render prompt texture */
		gPromptTexture.render(0, 0);


		/* update screen */
		SDL_RenderPresent(gRenderer);
	}

	/* free resources and close */
	close_sdl();

	return 0;
}






int
init_sdl(void)
{
	int flags;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
		fprintf(stderr, "SDL could not initialize! SDL Error: %s\n",
			SDL_GetError());
		return -1;
	}

	/* set texture filtering to linear */
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		fprintf(stderr,
			"Warning: Linear texture filtering not enabled!\n");

	/* create window */
	gWindow = SDL_CreateWindow("SDL Tutorial",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		fprintf(stderr, "window could not be created! SDL Error: %s\n",
			SDL_GetError());
		return -1;
	}

	/* create vsynced renderer for window */
	gRenderer = SDL_CreateRenderer(gWindow, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL) {
		fprintf(stderr, "Renderer could not be created! "
			"SDL Error: %s\n", SDL_GetError());
		return -1;
	} else {
		/* initialize renderer color */
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		/* initialize PNG loading */
		flags = IMG_INIT_PNG;
		if ((IMG_Init(flags) & flags) == 0) {
			fprintf(stderr, "SDL_image could not initialize! "
				"SDL_image Error: %s\n", IMG_GetError());
			return -1;
		}

		/* initialize SDL_mixer */
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			fprintf(stderr, "SDL_mixer could not initialize! "
				"SDL_mixer Error: %s\n", Mix_GetError());
			return -1;
		}


#ifdef _SDL_TTF_H
		/* initialize SDL_ttf */
		if (TTF_Init() == -1) {
			fprintf(stderr, "SDL_ttf could not initialize! "
				"SDL_ttf Error: %s\n", TTF_GetError());
			return -1;
		}
#endif
	}

	return 0;
}

int
load_media(void)
{
	/* load prompt texture */
	if (gPromptTexture.loadFromFile("prompt.png") != 0) {
		fprintf(stderr, "Failed to load prompt texture!\n");
		return -1;
	}

	/* load music */
	gMusic = Mix_LoadMUS("beat.wav");
	if (gMusic == NULL) {
		fprintf(stderr, "Failed to load music beat.wav! "
			"SDL_mixer Error: %s", Mix_GetError());
		return -1;
	}

	/* load sound effects */
	gScratch = Mix_LoadWAV("scratch.wav");
	if (gScratch == NULL) {
		fprintf(stderr, "Failed to load effect scratch.wav!"
			"SDL_mixer Error: %s\n", Mix_GetError());
		return -1;
	}

	gHigh = Mix_LoadWAV("high.wav");
	if (gScratch == NULL) {
		fprintf(stderr, "Failed to load effect high.wav!"
			"SDL_mixer Error: %s\n", Mix_GetError());
		return -1;
	}

	gMedium = Mix_LoadWAV("medium.wav");
	if (gScratch == NULL) {
		fprintf(stderr, "Failed to load effect medium.wav!"
			"SDL_mixer Error: %s\n", Mix_GetError());
		return -1;
	}

	gLow = Mix_LoadWAV("low.wav");
	if (gScratch == NULL) {
		fprintf(stderr, "Failed to load effect low.wav!"
			"SDL_mixer Error: %s\n", Mix_GetError());
		return -1;
	}

	return 0;
}


void
close_sdl(void)
{
	printf("close_sdl()\n");

	/* free loaded image */
	gPromptTexture.free();

	/* free the sound effects */
	Mix_FreeChunk(gScratch);
	Mix_FreeChunk(gHigh);
	Mix_FreeChunk(gMedium);
	Mix_FreeChunk(gLow);
	gScratch = NULL;
	gHigh = NULL;
	gMedium = NULL;
	gLow = NULL;

	/* free the music */
	Mix_FreeMusic(gMusic);
	gMusic = NULL;


	/* destroy window */
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	/* Quit SDL subsystem */
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}





