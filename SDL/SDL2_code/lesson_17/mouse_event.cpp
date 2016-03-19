/*
 * -lSDL2 -lSDL2_image
 */


#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_ttf.h>



/* screen dimension constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/* button constants */
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

enum LButtonSprite {
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3,
	BUTTON_SPRITE_TOTAL = 4
};


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


/* mouse button class */
class LButton
{
public:
	/* initializes internal variables */
	LButton();

	/* Sets top left position */
	void setPosition(int x, int y);

	/* handles mouse event */
	void handleEvent(SDL_Event *event, int i);

	/* shows button sprite */
	void render(int i);

private:
	/* top left position */
	SDL_Point mPosition;

	/* currently used global sprite */
	LButtonSprite mCurrentSprite;
};







/* the window we'll be rendering to */
SDL_Window *gWindow = NULL;

/* the window renderer */
SDL_Renderer *gRenderer = NULL;


/* mouse button sprites */
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gButtonSpriteSheetTexture;

/* button objects */
LButton gButtons[TOTAL_BUTTONS];







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


LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;

	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
	mPosition.x = x;
	mPosition.y = y;
}

void LButton::handleEvent(SDL_Event *event, int i)
{
	/* if mouse event happened */
	if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONUP
		|| event->type == SDL_MOUSEBUTTONDOWN)
	{
		/* get mouse position */
		int x, y;
		SDL_GetMouseState(&x, &y);

		printf("id = %d, x = %d, y = %d\n", i, x, y);

		/* check if mouse is in button */
		int inside = 1;

		/* mouse is left of the button */
		if (x < mPosition.x)
			inside = 0;

		/* mouse if right of the button */
		else if (x > mPosition.x + BUTTON_WIDTH)
			inside = 0;

		/* mouse above the button */
		else if (y < mPosition.y)
			inside = 0;

		/* mouse below the button */
		else if (y > mPosition.y + BUTTON_HEIGHT)
			inside = 0;


		/* mouse is outside button */
		if (!inside)
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;

		/* mouse is inside button */
		else {
			/* set mouse over sprite */
			switch (event->type) {
			case SDL_MOUSEMOTION:
				mCurrentSprite =
					BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;

			case SDL_MOUSEBUTTONDOWN:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			}
		}
	}
}

void LButton::render(int i)
{
	printf("render button %d\n", i);
	/* show current button sprite */
	gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y,
		&gSpriteClips[mCurrentSprite]);
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

			/* handle button events */
			for (int i = 0; i < TOTAL_BUTTONS; ++i)
				gButtons[i].handleEvent(&event, i);
		}

		/* clear screen */
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		/* render buttons */
		for (int i = 0; i < TOTAL_BUTTONS; ++i)
			gButtons[i].render(i);
		


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
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
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
	/* load sprites */
	if (gButtonSpriteSheetTexture.loadFromFile("button.png") == -1) {
		fprintf(stderr, "Failed to load sprite texture!\n");
		return -1;
	} else {
		/* set sprites */
		for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i) {
			gSpriteClips[i].x = 0;
			gSpriteClips[i].y = i * 200;
			gSpriteClips[i].w = BUTTON_WIDTH;
			gSpriteClips[i].h = BUTTON_HEIGHT;
		}

		/* set buttons in corners */
		gButtons[0].setPosition(0, 0);
		gButtons[1].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, 0);
		gButtons[2].setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);
		gButtons[3].setPosition(SCREEN_WIDTH - BUTTON_WIDTH,
					SCREEN_HEIGHT - BUTTON_HEIGHT);
	}

	return 0;
}


void
close_sdl(void)
{
	printf("close_sdl()\n");

	/* free loaded image */
	gButtonSpriteSheetTexture.free();


	/* destroy window */
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	/* Quit SDL subsystem */
	IMG_Quit();
	SDL_Quit();
}





