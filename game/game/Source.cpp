//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <sstream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();

	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture * mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
	
};
class Beer
{
public:
	//The dimensions of the beer
	static const int BEER_WIDTH = 16;
	static const int BEER_HEIGHT = 61;

	//Initializes the variables
	Beer();

	//Moves the beer
	void move();
	void stop();


	//Shows the beer on the screen relative to the camera
	void render();
	int getX();
	int getY();
	//SDL_Rect BeerPos;

private:
	//The X and Y offsets of the beer
	double mPosX, mPosY;

	//The velocity of the beer
	double mVelX, mVelY;
};
class Police
{
public:
	//The dimensions of the police
	static const int POLICE_WIDTH = 70;
	static const int POLICE_HEIGHT = 130;

	//Initializes the variables
	Police();

	//Moves the police
	void move();

	//Shows the police on the screen relative to the camera
	void render();
	int getX();
	int getY();
	

private:
	//The X and Y offsets of the police
	double mPosX, mPosY;

	//The velocity of the police
	double mVelX, mVelY;
};
class Car
{
public:
	//The dimensions of the car
	static const int CAR_WIDTH = 80;
	static const int CAR_HEIGHT = 70;
	//Maximum axis velocity of the car
	static const int CAR_VEL = 7;
	//Initializes the variables
	Car();
	//Takes key presses and adjusts the car's velocity
	void handleEvent(SDL_Event& e);
	//Moves the car
	void move();
	//Shows the car on the screen relative to the camera
	void render();
	int getX();
	int getY();

	//SDL_Rect CarPos;
private:
	//The X and Y offsets of the car
	int mPosX, mPosY;
	//The velocity of the car
	int mVelX, mVelY;
};
class LTimer
{
public:
	//Initializes variables
	LTimer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	Uint32 getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	Uint32 mStartTicks;

	//The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};


//Starts up SDL and creates window
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();
//Our test callback function
Uint32 moreBeermove(Uint32 x, void* p);
Uint32 morePolicemove(Uint32 u, void *w);
Uint32 moreBeerrender(Uint32 y, void *q);
Uint32 morePolicerender(Uint32 o, void*l);
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Globally used font
TTF_Font* gFont = NULL;
//The music that will be played
Mix_Music *gMusic = NULL;
Mix_Chunk *gEx = NULL;

//Scene textures
LTexture gCarTexture; //Car Texture
LTexture gBeerTexture; //Beer Texture
LTexture gPoliceTexture; //Police Texture
LTexture gBackgroundTexture; //Background Texture
LTexture gTimeTextTexture;
LTexture gStartPromptTexture;
LTexture gPausePromptTexture; // Timer Textures	
LTexture GameOverTex; //Game Over Texture which doesn't work 



LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}
LTexture::~LTexture()
{
	//Deallocate
	free();
}
bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}
#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	
	
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		
			
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
		
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif
void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}
void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}
void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}
void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}
int LTexture::getWidth()
{
	return mWidth;
}
int LTexture::getHeight()
{
	return mHeight;
}

LTimer::LTimer()
{
	//Initialize the variables
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}
void LTimer::start()
{
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}
void LTimer::stop()
{
	//Stop the timer
	mStarted = false;

	//Unpause the timer
	mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}
void LTimer::pause()
{
	//If the timer is running and isn't already paused
	if (mStarted && !mPaused)
	{
		//Pause the timer
		mPaused = true;

		//Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}
void LTimer::unpause()
{
	//If the timer is running and paused
	if (mStarted && mPaused)
	{
		//Unpause the timer
		mPaused = false;

		//Reset the starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		//Reset the paused ticks
		mPausedTicks = 0;
	}
}
Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if (mStarted)
	{
		//If the timer is paused
		if (mPaused)
		{
			//Return the number of ticks when the timer was paused
			time = mPausedTicks;
		}
		else
		{
			//Return the current time minus the start time
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}
bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
	return mStarted;
}
bool LTimer::isPaused()
{
	//Timer is running and paused
	return mPaused && mStarted;
}

Car::Car()
{
	//Initialize the offsets
	mPosX = 320;
	mPosY = 400;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
	
}
void Car::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= CAR_VEL; break;
		case SDLK_DOWN: mVelY += CAR_VEL; break;
		case SDLK_LEFT: mVelX -= CAR_VEL; break;
		case SDLK_RIGHT: mVelX += CAR_VEL; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += CAR_VEL; break;
		case SDLK_DOWN: mVelY -= CAR_VEL; break;
		case SDLK_LEFT: mVelX += CAR_VEL; break;
		case SDLK_RIGHT: mVelX -= CAR_VEL; break;
		}
	}
}
void Car::move()
{
	//Move the car left or right
	mPosX += mVelX;

	//If the car went too far to the left or right
	if ((mPosX < 0) || (mPosX + CAR_WIDTH > SCREEN_WIDTH - 125) || (mPosX + CAR_WIDTH <SCREEN_WIDTH - 435))
	{
		//Move bac
		mPosX -= mVelX;
	}


	//Move the car up or down
	mPosY += mVelY;

	//If the car went too far up or down
	if ((mPosY < 0) || (mPosY + CAR_HEIGHT > SCREEN_HEIGHT))
	{
		//Move back
		mPosY -= mVelY;
	}


}
void Car::render()
{
	//Show the car
	gCarTexture.render(mPosX, mPosY);
}
int Car::getX()
{
	return mPosX;
}
int Car::getY()
{
	return mPosY;
}

Beer::Beer()
{
	//Initialize the offsets
	mPosX = (rand() % 10)*30.0f + 155.0f;
	mPosY = 0;
	//Initialize the velocity
	mVelX = 0;
	mVelY = 2;
}
void Beer::move()
{
	mPosY += mVelY;

	if (SDL_GetTicks() > 5000 && SDL_GetTicks()<10000)
	{
		mPosY += 1;
	}
	else if (SDL_GetTicks() > 10000 && SDL_GetTicks()<15000)
	{
		mPosY += 2;
	}
	else if (SDL_GetTicks() > 15000&&SDL_GetTicks()<20000)
	{
		mPosY += 3;
	}
	else if (SDL_GetTicks() > 20000)
	{
		mPosY += 4;
	}
}
void Beer::render()
{

	gBeerTexture.render(mPosX, mPosY);

}
int Beer::getX()
{
	return   mPosX;
}
int Beer::getY()
{
	return mPosY;
}
void Beer::stop()
{
     mVelY=-10;
	 mPosY = mPosY;
	
}


Police::Police()
{
	//Initialize the offsets
	mPosX = (rand() % 10)*30.0f + 155.0f;
	mPosY = 0;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 2;
}
void Police::move()
{
	mPosY += mVelY;

	if (SDL_GetTicks() > 5000 && SDL_GetTicks()<10000)
	{
		mPosY += 1;
	}
	else if (SDL_GetTicks() > 10000 && SDL_GetTicks()<15000)
	{
		mPosY += 2;
	}
	else if (SDL_GetTicks() > 15000 && SDL_GetTicks()<20000)
	{
		mPosY += 3;
	}
	else if (SDL_GetTicks() > 20000)
	{
		mPosY += 4;
	}

}
void Police::render()
{

	gPoliceTexture.render(mPosX, mPosY);

}
int Police::getX()
{
	return   mPosX;
}
int Police::getY()
{
	return mPosY;
}


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Seed random
		srand(SDL_GetTicks());

		//Create window
		gWindow = SDL_CreateWindow("Uros Jankovic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load Car' texture
	if (!gCarTexture.loadFromFile("images/car.png"))
	{
		printf("Failed to load Foo' texture image!\n");
		success = false;
	}

	//Load background texture
	if (!gBackgroundTexture.loadFromFile("images/CarBackground2d.png"))
	{
		printf("Failed to load background texture image!\n");
		success = false;
	}
	//Load beer texture
	if (!gBeerTexture.loadFromFile("images/BeerFinalPic.png"))
	{
		printf("Failed to load background texture image!\n");
		success = false;
	}
	//Load police texture
	if (!gPoliceTexture.loadFromFile("images/PoliceFinalPic.png"))
	{
		printf("Failed to load background texture image!\n");
		success = false;
	}
	//Load GameOver texture
	if (!GameOverTex.loadFromFile("images/GameOver.png"))
	{
		printf("Failed to load background texture image!\n");
		success = false;
	}
	//Load music
	gMusic = Mix_LoadMUS("images/Wind.wav");
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	//Load music
	gEx = Mix_LoadWAV("images/Explosion.wav");
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	//Open the font
	gFont = TTF_OpenFont("images/lazy.ttf", 14);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Set text color as black
		SDL_Color textColor = { 255,255,255 };
		//Load pause prompt texture
		if (!gPausePromptTexture.loadFromRenderedText("P-Pause/Unpause", textColor))
		{
			printf("Unable to render pause/unpause prompt texture!\n");
			success = false;
		}
		//Load exit prompt texture
		if (!gStartPromptTexture.loadFromRenderedText("Press Esc to exit!", textColor))
		{
			printf("Unable to render pause/unpause prompt texture!\n");
			success = false;
		}

	
	}
		
	return success;
}

void close()
{
	//Free loaded images
	gCarTexture.free();
	gBeerTexture.free();
	gPoliceTexture.free();
	gBackgroundTexture.free();
	gTimeTextTexture.free();
	gPausePromptTexture.free();
	GameOverTex.free();


	//Free the sound effects
	Mix_FreeChunk(gEx);
	gEx = NULL;
	//Free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision(Beer* b, Car* car)
{

	std::vector<Beer*> beers;
	beers.push_back(b);
	//getting position of obj1
	//u need to have double (or int) function in your class to return x and y positions, width and height
	double ex = b->getX();
	double ey = b->getY();
	double ew = b->BEER_WIDTH;
	double eh = b->BEER_HEIGHT;
	//getting position of obj2
	double hx = car->getX();
	double hy =car->getY();
	double hw = car->CAR_WIDTH;
	double hh = car->CAR_HEIGHT;
	//Calculate the sides of obj1
	//in SDL position x and y are at left top corner of the tuxture, so u need to calculate max X position (right side) and max y position (bottom)
	double leftH = hx;
	double rightH = hx + hw;
	double topH = hy;
	double bottomH = hy + hh;
	//	Calculate the sides of obj2
	double leftE = ex;
	double rightE = ex + ew;
	double topE = ey;
	double bottomE = ey + eh;
	//check if objects are in different areas (no collision)
	if (bottomH <= topH)
	{
		return false;
	}
	if (topH >= bottomE)
	{
		return false;
	}
	if (rightH <= leftE)
	{
		return false;
	}
	if (leftH >= rightE)
	{
		return false;
	}
	//if there is a collision
	return true;
}//checks for collision of object of class Hero and object of class Weapon
bool checkCollisionPolice(Police* p, Car* car)
{
	std::vector<Police*>police;
	police.push_back(p);
	//getting position of police car
	double ex = p->getX();
	double ey = p->getY();
	double ew = p->POLICE_WIDTH;
	double eh = p->POLICE_HEIGHT;
	//getting position of player's car
	double hx = car->getX();
	double hy = car->getY();
	double hw = car->CAR_WIDTH;
	double hh = car->CAR_HEIGHT;
	//Calculate the sides of player's car
	double leftH = hx;
	double rightH = hx + hw;
	double topH = hy;
	double bottomH = hy + hh;
	//	Calculate the sides of police car
	double leftE = ex;
	double rightE = ex + ew;
	double topE = ey;
	double bottomE = ey + eh;
	//check if objects are in different areas (no collision)
	if (bottomH <= topH)
	{
		return false;
	}
	if (topH >= bottomE)
	{
		return false;
	}
	if (rightH <= leftE)
	{
		return false;
	}
	if (leftH >= rightE)
	{
		return false;
	}
	//if there is a collision
	return true;
}

//ERROR THAT THEY ARE RENDERED AT SAME POSITION EVERY TIME, AND IT RENDERS ONLY ONE OF EACH
//Beer b;
//Police p;

std::vector<Police>police;
std::vector<Beer>beers;
Uint32 moreBeermove(Uint32 x, void *p)
{
	
	for (auto it = beers.begin(); it != beers.end(); it++)
	{
		it->move();
	}	

	return 0;
}
Uint32 moreBeerrender(Uint32 y, void *q)
{
	
	for (auto it = beers.begin(); it != beers.end(); it++)
	{
		it->render();
	}
	return 0;
}
Uint32 morePolicemove(Uint32 u, void *w)
{
	for (auto it = police.begin(); it !=police.end(); it++)
	{
		it->move();
	}
	
	return 0;
}
Uint32 morePolicerender(Uint32 o, void*l)
{ 

	for (auto it = police.begin(); it != police.end(); it++)
	{
		it->render();
	}
	
	return 0;
}


int main(int argc, char* args[])
{

	srand(time(NULL));


	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Set text color as white
			SDL_Color textColor = { 255,255,255 };
			
			SDL_TimerID timerID = SDL_AddTimer(2000, moreBeermove, NULL);
			SDL_TimerID timerID1 = SDL_AddTimer(2000, moreBeerrender, NULL);
			SDL_TimerID timerID3 = SDL_AddTimer(1000, morePolicemove, NULL);
			SDL_TimerID timerID2 = SDL_AddTimer(1000, morePolicerender, NULL);
			
		


			//The car that will be moving around on the screen
			Car car;
			Police p;
			Beer b;
			beers.push_back(b);
			
			
			police.push_back(p);
			LTimer timer;
			Uint32 startTime = 0;
			std::stringstream timeText;
			std::stringstream gameover;
			
		

			timer.start();
			//While application is running
			while (!quit)
			{
				
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//Reset start time on return keypress
					else if (e.type == SDL_KEYDOWN)
					{

						//Pause/unpause
						 if (e.key.keysym.sym == SDLK_p)
						{
							 if (timer.isPaused())
							 {
								 Mix_ResumeMusic();
								 timer.unpause();
							 }
							 else { 
								 Mix_PauseMusic();
								 b.stop();
							 timer.pause(); 
							 }
						}
						
						 if (e.key.keysym.sym == SDLK_ESCAPE)
						 {
							 exit(0);
						 }
					
					}
					
					car.handleEvent(e);
				}
				
				if (Mix_PlayingMusic() == 0)
				{
					//Play the music
					Mix_PlayMusic(gMusic, -1);
				}

				//Move the car
				car.move();
				SDL_AddTimer(2000, moreBeermove, NULL);
				SDL_AddTimer(1000, morePolicemove, NULL);
				
				//If collision
				for (auto &b:beers)
				{
					if (checkCollision(&b, &car) == true)

					{
						beers.clear();
						startTime = timer.getTicks() + 3000;
					    
						std::cout << "Collision with beer! Plus 3000 points!" << std::endl;
					}
				}
				
				//If collision
				for (auto &p:police)
				{
					if (checkCollisionPolice(&p, &car) == true)
					{
						police.clear();
						Mix_PlayChannel(-1, gEx, 0);
						Mix_PauseMusic();
						for (auto&b : beers)
						{
							b.stop();
						}
						std::cout << "Collision with police!GAME OVER!" << std::endl;
						std::cout << "Your score is:" << timer.getTicks() + startTime << std::endl;
						timer.stop();
						GameOverTex.render(0, 0); //Doesn't work
					}
					
				}
			
		
						
				//Set text to be rendered-startTime
				timeText.str("");
				timeText << "Score:" << timer.getTicks()+startTime;
				//Render text
				if (!gTimeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor))
				{
					printf("Unable to render time texture!\n");
				}

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render background texture to screen
				gBackgroundTexture.render(0, 0);
				//Render car to the screen
				car.render();
				for (auto it = police.begin(); it != police.end(); it++)
				{
					it->render();
				}
				
				for (auto it = beers.begin(); it != beers.end(); it++)
				{
					it->render();
				}
			
				
				gStartPromptTexture.render(30, 40);
				gPausePromptTexture.render(30, 20);
				gTimeTextTexture.render(30, 0);
	
				//Update screen
				SDL_RenderPresent(gRenderer);
	
			}
			SDL_RemoveTimer(timerID);
			SDL_RemoveTimer(timerID1);
			SDL_RemoveTimer(timerID2);
			SDL_RemoveTimer(timerID3);
		}
	} 

	//Free resources and close SDL
	close();

	system("pause");


	return 0;

}
