#include "Window.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <SDL.h>


//using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

LWindow::LWindow()
{
	//Initialize non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mTracking = false;
	isZoomed = false;
	mWidth = 0;
	mHeight = 0;
	Spectra s("export.txt");
	spectra = s;

	zOriginX = 0;
	zOriginY = 0;
	zWidth = 0;
	zHeight = 0;

	xOffset = 0;
	yOffset = 0;
}

void LWindow::draw()
{
	//std::cout << spectra.count;
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	if (!isZoomed) {
		spectra.render(gRenderer);
	}
	else {
		std::cout << "calling draw iwth yoffset " << yOffset << "\n";
		spectra.render(gRenderer, xOffset, yOffset);
	}
	

	if (mTracking) {
		SDL_Rect viewPort = { 0, 0, mWidth, mHeight };
		SDL_RenderSetViewport(gRenderer, &viewPort);
		SDL_Rect outlineRect = { zOriginX, zOriginY, zWidth, zHeight };
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawRect(gRenderer, &outlineRect);
	}

	SDL_RenderPresent(gRenderer);

}

bool LWindow::init()
{
	//Create window
	mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (mWindow != NULL)
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
	}



	return mWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer()
{
	gRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	return gRenderer;

}

void LWindow::handleEvent(SDL_Event& e)
{
	//Caption update flag
	bool updateCaption = false;

	//mouse position
	int mx = 0;
	int my = 0;

	switch (e.type) {
	case SDL_WINDOWEVENT:
		switch (e.window.event){
			//Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:

			mWidth = e.window.data1;
			mHeight = e.window.data2;
			draw();
			break;

			//Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			draw();
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}
		break;

	case SDL_DROPFILE: {
		std::string line;
		std::ifstream myfile(e.drop.file);
		if (myfile.is_open()){
			printf("opened file");
		}}
		break;

	case SDL_MOUSEMOTION:
		SDL_GetMouseState(&mx, &my);
		if (mTracking){
			zWidth = mx - zOriginX;
			zHeight = my - zOriginY;
		}
		updateCaption = true;
		draw();
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT) {
			SDL_GetMouseState(&mx, &my);
			zWidth = 0;
			zHeight = 0;
			zOriginX = mx;
			zOriginY = my;
			mTracking = true;
		}
		else {
			spectra.unzoom(mWidth, mHeight);
			isZoomed = false;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (e.button.button == SDL_BUTTON_LEFT) {
			SDL_GetMouseState(&mx, &my);
			std::cout << "zooming to region " << spectra.time(zOriginX) << " " << spectra.time(mx) << " " << spectra.intensity(my) << " " << spectra.intensity(zOriginY) << "\n";
			xOffset = spectra.time(zOriginX);
			yOffset = spectra.intensity(my);
			spectra.setXYFactor(mWidth, mHeight, spectra.time(zOriginX), spectra.time(mx), spectra.intensity(my), spectra.intensity(zOriginY));
			isZoomed = true;
			mTracking = false;
			draw();
		}
		break;
	}


	//Update window caption with new data
	if (updateCaption)
	{
		std::stringstream caption;
		caption << "Time:  " << spectra.time(mx) << "  I:  " << spectra.intensity(my);
		
		SDL_SetWindowTitle(mWindow, caption.str().c_str());
	}

}

void LWindow::free()
{
	if (mWindow != NULL)
	{
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
	return mWidth;
}

int LWindow::getHeight()
{
	return mHeight;
}

bool LWindow::hasMouseFocus()
{
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
	return mMinimized;
}