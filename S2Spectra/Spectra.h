//
//  Spectra.hpp
//  S2Spectra
//
//  Created by Joshua Knapp on 2/9/16.
//  Copyright © 2016 SnTwo. All rights reserved.
//

#ifndef Spectra_hpp
#define Spectra_hpp

#include <stdio.h>
#include <string>
#include "Texture.h"
//#include <SDL.h>

struct SDL_Renderer;

struct Integration{

	int startIndex = 0;
	int endIndex = 0;
	float area = 0;
	bool isSet = false;
	float time;
	LTexture areaLabel;
	LTexture timeLabel;
	LTexture areaPercentLabel;
	std::string labelString;
	bool labelIsSet = false;

};

class Spectra {

public:
	Spectra();
	Spectra(std::string);
	float times[50000], intensities[50000];
	Integration integrations[50];
	
	float maxX = 0;
	float maxY = 0;
	float minX = 0;
	float minY = 0;
	long count = 0;
	std::string name;
	
	float xFactor = 1;
	float yFactor = 1;
	float xOffset = 0;
	float yOffset = 0;

	float time(int);
	float intensity(int);

	

	bool isLoaded = false;
	bool isKey = false;

	int integrate(float, float);
	int getTimeIndexForTime(float);
	void redoStrings();

	
};

#endif /* Spectra_hpp */