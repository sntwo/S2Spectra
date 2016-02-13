//
//  main.cpp
//  S2Spectra
//
//  Created by Joshua Knapp on 2/9/16.
//  Copyright © 2016 SnTwo. All rights reserved.
//


//#include <SDL2/SDL.h>
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>


#include "Window.h"

using namespace std;




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

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Our custom window
LWindow gWindow;




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


int main(int argc, char* args[])
{
	printf("Hello world");
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

			Spectra spectra("export.txt");

			if (!gWindow.isMinimized()){

				gWindow.draw();

			}
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
			}



		}
	}

	//Free resources and close SDL
	close();

	return 0;
}