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
//#include <SDL.h>

struct SDL_Renderer;

class Spectra {

public:
	Spectra();
	Spectra(std::string);
	float times[50000], intensities[50000];
	float maxX = 0;
	float maxY = 0;
	float minX = 0;
	float minY = 0;
	long count = 0;
	void render(SDL_Renderer*);
	void render(SDL_Renderer*, float, float);
	void setXYFactor(float, float, float, float, float, float);
	float xFactor = 1;
	float yFactor = 1;
	float xOffset = 0;
	float yOffset = 0;

	float time(int);
	float intensity(int);

	void unzoom(float, float);
};

#endif /* Spectra_hpp */