/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

#include "Window.h"


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//Our custom window
LWindow gWindow;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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

		//Create window
		if (!gWindow.init())
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = gWindow.createRenderer();
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	//SDL_DestroyWindow(gWindow);
	gWindow.free();
	//gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

class Spectra {
	
public:
	Spectra(string);
	float times[50000], intensities[50000];
	float maxX = 0;
	float maxY = 0;
	float minX = 0;
	float minY = 0;
	long count;
	void render (float, float);
};

void Spectra::render (float screenWidth, float screenHeight){
	float h = (float)gWindow.getHeight() - 50;
	float xFactor = screenWidth / maxX;
	float yFactor = screenHeight / ((maxY - minY) * 1.5f); //pad out the y dimension a bit

	for (int i = 0; i < count; i++){
		float x1 = times[i] * xFactor;
		float y1 = h - intensities[i] * yFactor;
		float x2 = times[i + 1] * xFactor;
		float y2 = h - intensities[i + 1] * yFactor;
		SDL_RenderDrawLine(gRenderer, x1, y1, x2, y2);
		//cout << "drew point at " << times[i] * xFactor << intensities[i] * yFactor << "\n";
	}
}

Spectra::Spectra(string fileName) {

	string line;
	ifstream myfile(fileName);
	if (myfile.is_open()){

		getline(myfile, line);  // chromatogram
		cout << line << "\n";
		getline(myfile, line);  // name
		cout << line << "\n";
		getline(myfile, line);  // data points
		cout << line << "\n";
		istringstream dp(line);
		string skip;

		dp >> skip >> skip >> count;
		cout << "count: " << count << "\n";
		getline(myfile, line);  // time & intensity

		int idx = 0;
		while (getline(myfile, line)){

			istringstream iss(line);
			float x;
			float y;
			iss >> x >> y;
			times[idx] = x;
			intensities[idx] = y;
			//cout << "x:" << x << "y:" << y << '\n';

			if (x > maxX) { maxX = x; }
			if (x < minX) { minX = x; }
			if (y > maxY) { maxY = y; }
			if (y < minY) { minY = y; }

			idx++;
		}
		myfile.close();

		cout << "maxX is " << maxX << "\n";
		cout << "maxY is " << maxY << "\n";
	}
}

int main(int argc, char* args[])
{
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

			Spectra spec("export.txt");

			//While application is running
			while (!quit){
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0){
					//User requests quit
					if (e.type == SDL_QUIT){
						quit = true;
					}

					gWindow.handleEvent(e);
				}

				if (!gWindow.isMinimized()){

					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					////Render red filled quad
					//SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
					//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
					//SDL_RenderFillRect(gRenderer, &fillRect);

					////Render green outlined quad
					//SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
					//SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
					//SDL_RenderDrawRect(gRenderer, &outlineRect);

					////Draw blue horizontal line
					//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
					//SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
					float w = (float)gWindow.getWidth();
					float h = (float)gWindow.getHeight();
					spec.render(w, h);

					////Draw vertical line of yellow dots
					//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
					//for (int i = 0; i < SCREEN_HEIGHT; i += 4)
					//{
					//	SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH / 2, i);
					//}

					//Update screen
					SDL_RenderPresent(gRenderer);
				}
			}

				
			
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}