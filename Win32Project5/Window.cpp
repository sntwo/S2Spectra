#include "Window.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <SDL.h>
#include <SDL_ttf.h>

//using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Rect viewport;

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
	//Spectra s("export.txt");
	//spectra = s;

	//spectra.unzoom(640.f, 480.f);

	zOriginX = 0;
	zOriginY = 0;
	zWidth = 0;
	zHeight = 0;

	xOffset = 0;
	yOffset = -500000;

	minIntensity = -200000.f;
	maxIntensity = 2000000.f;
	timeStart = 0.f;
	timeEnd = 30.f;

	xFactor = 1;
	yFactor = 1;
}

void LWindow::setColor(int i) {
	switch (i) {
	case 0:
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF); // yellow
		break;
	case 1:
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF); // red
		break;
	case 2:
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); // black
		break;
	case 3:
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF); // green
		break;
	}
}

void LWindow::render(int idx)
{
	
	for (int i = 0; i < spectras[idx].count; i++){
		//intf("looping");
		float x1 = (spectras[idx].times[i] - xOffset) * xFactor;
		float y1 = viewport.h - (spectras[idx].intensities[i] - yOffset) * yFactor;
		float x2 = (spectras[idx].times[i + 1] - xOffset) * xFactor;
		float y2 = viewport.h - (spectras[idx].intensities[i + 1] - yOffset) * yFactor;
		if (x2 < x1) { /* printf("found backwards line");*/ }
		
 		else {
			SDL_RenderDrawLine(gRenderer, x1, y1, x2, y2);
		}
		//d::cout << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
	}
	

}

void LWindow::draw()
{
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	
	viewport.x = 10;
	viewport.y = 10;
	viewport.w = mWidth - 20;
	viewport.h = mHeight / 1.6;
	SDL_RenderSetViewport(gRenderer, &viewport);
	SDL_Rect outline = { 0, 0, viewport.w, viewport.h };
	SDL_RenderDrawRect(gRenderer, &outline);
	
	xFactor = viewport.w / (abs(timeEnd - timeStart));
	yFactor = viewport.h / ((abs(maxIntensity - minIntensity)) * 1.2); //pad out the y dimension a bit

	for (int i = 0; i < 6; i++){
		if (spectras[i].isLoaded) {
			setColor(i);
			render(i);
		}
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

float LWindow::time(int x){
	return (x - viewport.x) / xFactor + xOffset;
}

float LWindow::intensity(int y){
	y -= viewport.y;
	return yOffset - (y - viewport.h) / yFactor;
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
		int i = 0;
		for (; i < 6; i++){
			if (!spectras[i].isLoaded) {
				break;
			}
		}
		Spectra s(e.drop.file);
		spectras[i] = s;
		}
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
			xOffset = 0;
			yOffset = -20000;
			minIntensity = -200000.f;
			maxIntensity = 2000000.f;
			timeStart = 0.f;
			timeEnd = 30.f;
			isZoomed = false;
		}
		break;

	case SDL_MOUSEBUTTONUP:
		if (e.button.button == SDL_BUTTON_LEFT) {
			SDL_GetMouseState(&mx, &my);
			//std::cout << "zooming to region " << spectra.time(zOriginX) << " " << spectra.time(mx) << " " << spectra.intensity(my) << " " << spectra.intensity(zOriginY) << "\n";

			//spectras[0].setXYFactor(mWidth, mHeight, spectras[0].time(zOriginX), spectras[0].time(mx), spectras[0].intensity(my), spectras[0].intensity(zOriginY));
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
		caption << "Time:  " << time(mx) << "  I:  " << intensity(my);
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