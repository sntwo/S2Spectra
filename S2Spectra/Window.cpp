//#include <SDL2/SDL.h>

#include "Window.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <algorithm>

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

//#include <SDL_ttf.h>

//using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;

SDL_Rect viewport;

//Globally used font
TTF_Font *gFont = NULL;
TTF_Font *gFontBold = NULL;
LTexture gTextTexture;

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
    xSmoother = 0;

	drawMode = zoom;


}

void LWindow::setColor(int i) {
	switch (i) {
	case 0:
		SDL_SetRenderDrawColor(gRenderer, 0x00, 64, 255, 0xFF); // blue
		break;
	case 1:
		SDL_SetRenderDrawColor(gRenderer, 0, 255, 64, 0xFF); // green
		break;
	case 2:
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 0xFF); // orange
		break;
	case 3:
		SDL_SetRenderDrawColor(gRenderer, 191, 0, 255, 0xFF); // purp;e
		break;
    case 4:
		SDL_SetRenderDrawColor(gRenderer, 64, 0, 255, 0xFF); // red
        break;
    case 5:
        SDL_SetRenderDrawColor(gRenderer, 153, 102, 0, 0xFF); // brown?
        break;
	}
}

SDL_Color LWindow::getColor(int i){
	//std::cout << "supplying color for int " << i << "\n";
	SDL_Color textColor;
	switch (i) {
	case 0:
		//std::cout << "supplying yellow \n";
		textColor = { 0, 1, 1 }; // yellow
		break;
	case 1:
		textColor = { 1, 0, 0 }; // red
		break;
	case 2:
		textColor = { 0, 0, 0 }; // black
		break;
	case 3:
		textColor = { 0, 1, 0 }; // green
		break;
	}
	return textColor;
}

void LWindow::render(int idx)
{
    //calculate xSmoother to skip times when they are closer than a pixel
    float dif = timeEnd - timeStart;
    float percent = dif / spectras[idx].maxX;
    float n = spectras[idx].count * percent;
    xSmoother = (int)(0.5 / (viewport.w / n) - 5);
    if (xSmoother < 0) { xSmoother = 0 ;}
    
    //loop through spectra times and draw lines
	for (int i = 0; i < spectras[idx].count; i++){
        while ( spectras[idx].times[i] < timeStart ) { i++; }
        if ( spectras[idx].times[i] > timeEnd ) { break; }
        if ( spectras[idx].intensities[i] < maxIntensity || spectras[idx].intensities[i + xSmoother + 1] < maxIntensity) {
            if (spectras[idx].intensities[i] > minIntensity || spectras[idx].intensities[i + xSmoother + 1] > minIntensity) {
        
                float x1 = (spectras[idx].times[i] - xOffset) * xFactor;
                float y1 = viewport.h - (spectras[idx].intensities[i] - yOffset) * yFactor;
                float x2 = (spectras[idx].times[i + xSmoother + 1] - xOffset) * xFactor;
                float y2 = viewport.h - (spectras[idx].intensities[i + xSmoother + 1] - yOffset) * yFactor;
                if (x2 < x1) { /* printf("found backwards line");*/ }

                else {
                    SDL_RenderDrawLine(gRenderer, x1, y1, x2, y2);
                }
            }
        }
        i += xSmoother;
        
	}

	//render nametag
	nameTextures[idx].render(gRenderer, viewport.w - 200, 20 * idx);
	SDL_RenderDrawLine(gRenderer, viewport.w - 200 - 30, 20 * idx + 8.5, viewport.w - 200 - 10, 20 * idx + 8.5);

	//render integrations on key spectra
	if (!spectras[idx].isKey) { return; }
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF); // red
	for (int i = 0; i < 50; i++){
		if (spectras[idx].integrations[i].isSet) {
			//printf("found integration to draw \n");

			int xi = spectras[idx].integrations[i].startIndex;
			int xf = spectras[idx].integrations[i].endIndex;
			//std::cout << "start index is " << xi << "\n";
			//std::cout << "end index is " << xf << "\n";

			float x1 = (spectras[idx].times[xi] - xOffset) * xFactor;
			float y1 = viewport.h - (spectras[idx].intensities[xi] - yOffset) * yFactor;
			float x2 = (spectras[idx].times[xf + xSmoother + 1] - xOffset) * xFactor;
			float y2 = viewport.h - (spectras[idx].intensities[xf + xSmoother + 1] - yOffset) * yFactor;

			SDL_RenderDrawLine(gRenderer, x1, y1, x2, y2);
		}
	}


}

void LWindow::draw()
{
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	if (mTracking) {
		if (drawMode == zoom) {
			SDL_Rect viewPort = { 0, 0, mWidth, mHeight };
			SDL_RenderSetViewport(gRenderer, &viewPort);
			SDL_Rect outlineRect = { zOriginX, zOriginY, zWidth, zHeight };
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
			SDL_RenderDrawRect(gRenderer, &outlineRect);
		}
		else if (drawMode == integrate) {
			SDL_Rect viewPort = { 0, 0, mWidth, mHeight };
			SDL_RenderSetViewport(gRenderer, &viewPort);
			SDL_Rect outlineRect = { zOriginX, viewport.y, zWidth, viewport.h };
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xC8, 0x00, 0xFF);
			SDL_RenderFillRect(gRenderer, &outlineRect);
		}
		else if (drawMode == deintegrate) {
			SDL_Rect viewPort = { 0, 0, mWidth, mHeight };
			SDL_RenderSetViewport(gRenderer, &viewPort);
			SDL_Rect outlineRect = { zOriginX, viewport.y, zWidth, viewport.h };
			SDL_SetRenderDrawColor(gRenderer, 0xC8, 0x00, 0x00, 0xFF);
			SDL_RenderFillRect(gRenderer, &outlineRect);
		}
	}

	else if (drawMode == integrate) {
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_Rect viewPort = { 0, 0, mWidth, mHeight };
		SDL_RenderSetViewport(gRenderer, &viewPort);
		SDL_RenderDrawLine(gRenderer, mx, viewport.y, mx, viewport.h + viewport.y);
	}
	else if (drawMode == deintegrate) {
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_Rect viewPort = { 0, 0, mWidth, mHeight };
		SDL_RenderSetViewport(gRenderer, &viewPort);
		SDL_RenderDrawLine(gRenderer, mx, viewport.y, mx, viewport.h + viewport.y);
	}

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	viewport.x = 10;
	viewport.y = 10;
	viewport.w = mWidth - 20;
	viewport.h = mHeight / 1.6;
	SDL_RenderSetViewport(gRenderer, &viewport);
	SDL_Rect outline = { 0, 0, viewport.w, viewport.h };
	SDL_RenderDrawRect(gRenderer, &outline);

	xFactor = viewport.w / (fabs(timeEnd - timeStart));
    yFactor = viewport.h / ((fabs(maxIntensity - minIntensity)) ); //pad out the y dimension a bit

	for (int i = 0; i < 6; i++){
		if (spectras[i].isLoaded) {
			setColor(i);
			render(i);
		}
	}

	SDL_Rect integrationTable;
	integrationTable.x = 10;
	integrationTable.y = mHeight / 1.6 + 10;
	integrationTable.w = mWidth - 20;
	integrationTable.h = mHeight - mHeight / 1.6 - 10;

	SDL_RenderSetViewport(gRenderer, &integrationTable);

	timeLabel.render(gRenderer, 0, 10);
	areaLabel.render(gRenderer, 200, 10);
	areaPercentLabel.render(gRenderer, 400, 10);

	for (int i = 0; i < 6; i++){
		if (spectras[i].isKey) {
			for (int j = 0; j < 50; j++){
				if (spectras[i].integrations[j].isSet) {
					spectras[i].integrations[j].timeLabel.render(gRenderer, 0, 20 * j + 30);
					spectras[i].integrations[j].areaLabel.render(gRenderer, 200, 20 * j + 30);
					spectras[i].integrations[j].areaPercentLabel.render(gRenderer, 400, 20 * j + 30);
				}
			}
		}
	}


	SDL_RenderPresent(gRenderer);

}

float LWindow::time(int x){
	x -= viewport.x;
	return x / xFactor + xOffset;
}

float LWindow::intensity(int y){
	y -= viewport.y;
	return (viewport.h - y) / yFactor + yOffset;
}

bool LWindow::init()
{
	//Create window
	mWindow = SDL_CreateWindow("S2Spectra", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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


	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		//success = false;
	}

	//Open the fonts
	gFont = TTF_OpenFont("OpenSans-Light.ttf", 12);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		//success = false;
	}
	gFontBold = TTF_OpenFont("OpenSans-Bold.ttf", 12);
	if (gFontBold == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		//success = false;
	}


	SDL_Color tc = getColor(2);
	std::string a = "Area";
	std::string b = "Time";
	std::string c = "Area Percent";
	if (!areaLabel.loadFromRenderedText(a, tc, gRenderer, gFont)){

	}
	if (!areaPercentLabel.loadFromRenderedText(c, tc, gRenderer, gFont)){

	}
	if (!timeLabel.loadFromRenderedText(b, tc, gRenderer, gFont)){

	}


	return gRenderer;

}

void LWindow::handleEvent(SDL_Event& e)
{
	//Caption update flag
	bool updateCaption = false;

	switch (e.type) {
	case SDL_KEYDOWN:
		if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
			//found paste attempt
			std::string inputText = SDL_GetClipboardText(); 
			int i = 0;
			for (; i < 6; i++){
				if (!spectras[i].isLoaded) {
					break;
				}
			}

			//printf("trying to paste file \n");
			//std::cout << "file is " << inputText << "\n";
			spectras[i].loadFromString(inputText);
			makeKey(i);
		}

		else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
			//copy attempt
            printf("trying to copy");
			std::string outString = "";
			for (int i = 0; i < 6; i++){
				if (spectras[i].isLoaded) {
					
					outString += spectras[i].name;
					outString += "\n";
					outString += "Time\tArea\tArea %\n";
					
					for (int j = 0; j < 50; j++){
						if (spectras[i].integrations[j].isSet){
							outString += std::to_string(spectras[i].integrations[j].time);
							outString += "\t";
							outString += std::to_string(spectras[i].integrations[j].area);
							outString += "\t";
							outString += spectras[i].integrations[j].labelString;
							outString += "\n";
						}
					}
                    outString += "\n";
				}
			}
            
            float timesList[50];
            for (int i = 0; i < 50; i++) {
                timesList[i] = 10000.f;
            }
            
            outString += "Impurity trend: \nTime\t";
            for (int i = 0; i < 6; i++){
                if (spectras[i].isLoaded) {
                    outString += spectras[i].name;
                    outString += "\t";
                }
            }
        
            outString += "\n";
            int filled = 0;
            for (int i = 0; i < 6; i++){
                if (spectras[i].isLoaded) {
                    for (int j = 0; j < 50; j++) {
                        if (spectras[i].integrations[j].isSet) {
                            bool found = false;
                            for (int k = 0; k < 50; k++) {
                                if (fabs(spectras[i].integrations[j].time - timesList[k]) < 0.2) {
                                    found = true;
                                }
                            }
                            if (!found){
                                timesList[filled] = spectras[i].integrations[j].time;
                                filled++;
                            }
                        }
                    }
                }
            }
            
            std::sort(timesList, timesList + 50);
            
            for (int i = 0; i < 50; i++){
                outString += std::to_string(timesList[i]);
                outString += "\t";
                for (int j = 0; j < 6; j++) {
                        if (spectras[j].isLoaded) {
                            bool found = false;
                            for (int k = 0; k < 50; k++) {
                                if (spectras[j].integrations[k].isSet) {
                                    
                                    if (fabs(spectras[j].integrations[k].time - timesList[i]) < 0.2) {
                                        outString += spectras[j].integrations[k].labelString;
                                        outString += "\t";
                                        found = true;
                                    }
                                }
                            }
                            if (!found) {
                                outString += "--\t";
                            }
                        }
                }
                outString += "\n";
               
            }
			
			SDL_SetClipboardText(outString.c_str());
		
		}

		switch (e.key.keysym.sym) {
		case SDLK_MINUS:
			//printf("unzooming");
			xOffset = 0;
			yOffset = -500000;
			minIntensity = -200000.f;
			maxIntensity = 2000000.f;
			timeStart = 0.f;
			timeEnd = 30.f;
			isZoomed = false;
			break;
		case SDLK_i:
			drawMode = integrate;
			break;

		case SDLK_z:
			drawMode = zoom;
			break;

		case SDLK_x:
			drawMode = deintegrate;
			break;
		}

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
		makeKey(i);

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
		SDL_GetMouseState(&mx, &my);

		if (e.button.button == SDL_BUTTON_LEFT) {
			int check = checkNameBoxes(mx, my);
			if (check > -1) {
				makeKey(check);
			}
			else {
				mTracking = true;
				zWidth = 0;
				zHeight = 0;
				zOriginX = mx;
				zOriginY = my;
			}
		}

		else {
			//printf("unzooming");
			xOffset = 0;
			yOffset = -500000;
			minIntensity = -200000.f;
			maxIntensity = 2000000.f;
			timeStart = 0.f;
			timeEnd = 30.f;
			isZoomed = false;
		}
		break;

	case SDL_MOUSEBUTTONUP:
		if (e.button.button == SDL_BUTTON_LEFT) {
			if (!mTracking) break;
			mTracking = false;
			SDL_GetMouseState(&mx, &my);
			switch (drawMode){
			case zoom:
				//std::cout << "zooming to region " << time(zOriginX) << " " << time(mx) << " " << intensity(my) << " " << intensity(zOriginY) << "\n";
				timeStart = time(zOriginX);
				timeEnd = time(mx);
				minIntensity = intensity(my);
				maxIntensity = intensity(zOriginY);
				xOffset = timeStart;
				yOffset = minIntensity;
				isZoomed = true;
				draw();
				break;

			case integrate: {
				for (int i = 0; i < 6; i++){
					if (spectras[i].isKey){
						spectras[i].integrate(time(zOriginX), time(mx));
                        
					}
				}
				redoStrings();
				draw();
				break;
			}
			case deintegrate:
				for (int i = 0; i < 6; i++){
					if (spectras[i].isKey){
						spectras[i].deleteRange(time(zOriginX), time(mx));
                        
					}
				}
				redoStrings();
				draw();
				break;
			}

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

void LWindow::redoStrings() {
    SDL_Color tc = getColor(2);
    for (int i = 0; i < 6; i++){
        if (spectras[i].isKey){
            for (int j = 0; j < 50; j++){
                if (spectras[i].integrations[j].isSet){
                    std::string time = "";
                    std::string area = "";
                    time += std::to_string(spectras[i].integrations[j].time);
                    area += std::to_string(spectras[i].integrations[j].area);
                    if (!spectras[i].integrations[j].timeLabel.loadFromRenderedText(time, tc, gRenderer, gFont)){
                    }
                    if (!spectras[i].integrations[j].areaLabel.loadFromRenderedText(area, tc, gRenderer, gFont)){
                    }
                    if (!spectras[i].integrations[j].areaPercentLabel.loadFromRenderedText(spectras[i].integrations[j].labelString, tc, gRenderer, gFont)){
                    }
                    spectras[i].integrations[j].labelIsSet = true;
                }
            }
        }
    }
}

void LWindow::makeKey(int idx) {
	for (int i = 0; i < 6; i++){

		if (!spectras[i].isLoaded) {
			break;
		}
		if (i != idx) {
			spectras[i].isKey = false;
			if (nameTextures[i].isLoaded) {

				SDL_Color tc = getColor(i);
				nameTextures[i].isLoaded = false;
				//nameTextures[i].free();
				if (!nameTextures[i].loadFromRenderedText(spectras[i].name, tc, gRenderer, gFont))
				{
					printf("Failed to render non bold text texture!\n");
					//success = false;
				}
				else {
					nameTextures[i].isLoaded = true;
				}
			}
		}
		else {
			spectras[i].isKey = true;
            spectras[i].redoStrings();
            redoStrings();
			if (nameTextures[i].isLoaded) {
				nameTextures[i].isLoaded = false;
				//nameTextures[i].free();
			}
			SDL_Color tc = getColor(i);
			if (!nameTextures[i].loadFromRenderedText(spectras[i].name, tc, gRenderer, gFontBold))
			{
				printf("Failed to render non bold text texture!\n");
				//success = false;
			}
			else {
				nameTextures[i].isLoaded = true;
			}
		}
	}
}

int LWindow::checkNameBoxes(int x, int y){
	//std::cout << "checking " << x << " " << y << "\n";
	for (int i = 0; i < 6; i++){
		if (spectras[i].isLoaded) {
			if (x > (viewport.w - 230) && y < (20 * i + 25) && y >(20 * i + 5)){
				return i;
			}
		}
	}
	return -1;
}

void LWindow::free()
{
	if (mWindow != NULL)
	{
		SDL_DestroyWindow(mWindow);
	}

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;
	TTF_CloseFont(gFontBold);
	gFontBold = NULL;

	TTF_Quit();

	SDL_Quit();

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