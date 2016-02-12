//
//  Spectra.cpp
//  S2Spectra
//
//  Created by Joshua Knapp on 2/9/16.
//  Copyright © 2016 SnTwo. All rights reserved.
//

#include "Spectra.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>


using namespace std;

Spectra::Spectra(){}

//SDL_Rect viewport;

float xOffset = 0;
float yOffset = 0;

//void Spectra::getDisplayParameters(float &xOff, float &yOff, float &xFact, float &yFact){
//	xOff = xOffset;
//	yOff = yOffset;
//	xFact = xFactor;
//	yFact = yFactor;
//}
//
//void Spectra::setDisplayParamaters(float xOff, float yOff, float xFact, float yFact){
//
//	xOffset = xOff;
//	yOffset = yOff;
//	xFactor = xFact;
//	yFactor = yFact;
//}

//void Spectra::render(SDL_Renderer* renderer){
//
//	SDL_Rect outline;
//	outline.x = 0;
//	outline.w = viewport.w;
//	outline.y = 0;
//	outline.h = viewport.h;
//	SDL_RenderSetViewport(renderer, &viewport);
//	SDL_RenderDrawRect(renderer, &outline);
//
//
//	for (int i = 0; i < count; i++){
//		float x1 = (times[i] - xOffset) * xFactor;
//		float y1 = viewport.h - (intensities[i] - yOffset) * yFactor - 30;
//		float x2 = (times[i + 1] - xOffset) * xFactor;
//		float y2 = viewport.h - (intensities[i + 1] - yOffset) * yFactor - 30;
//		if (x2 < x1) { /* printf("found backwards line");*/ }
//		else {
//			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
//		}
//	}
//}

//void Spectra::setXYFactor(float screenWidth, float screenHeight, float xMin, float xMax, float yMin, float yMax){
//	printf("set xy factor");
//	cout << "xMin is " << xMin << "  xMax is " << xMax << "\n";
//
//	xOffset = xMin;
//	yOffset = yMin;
//	viewport.x = 10;
//	viewport.y = 10;
//	viewport.w = screenWidth - 20;
//	viewport.h = screenHeight / 1.6;
//
//	xFactor = viewport.w / (abs(xMax - xMin));
//	yFactor = viewport.h / ((abs(yMax - yMin)) * 1.2); //pad out the y dimension a bit
//	cout << "xFactor is " << xFactor << "  yFactor is " << yFactor << "\n";
//}

//float Spectra::time(int x){
//	return (x - viewport.x) / xFactor + xOffset;
//}
//
//float Spectra::intensity(int y){
//	return yOffset - (y + 30 - viewport.h) / yFactor;
//}

Spectra::Spectra(std::string fileName) {
    
    
    
    string line;
    ifstream myfile(fileName);
    if (myfile.is_open()){
        
        getline(myfile, line);  // chromatogram
        cout << line << "\n";
        getline(myfile, name);  // name
        cout << name << "\n";
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
        cout << "minX is " << minX << "\n";
        cout << "maxY is " << maxY << "\n";
        cout << "minY is " << minY << "\n";
    }
    
    isLoaded = true;
}

//void Spectra::unzoom(float screenWidth, float screenHeight) {
//	setXYFactor(screenWidth, screenHeight, minX, maxX, minY, maxY);
//}