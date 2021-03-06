//
//  Texture.hpp
//  S2Spectra
//
//  Created by Joshua Knapp on 2/11/16.
//  Copyright � 2016 SnTwo. All rights reserved.
//

#ifndef Texture_hpp
#define Texture_hpp

//#include <SDL2/SDL.h>
//#include <SDL2_image/SDL_image.h>
//#include <SDL_image.h>
//#include <SDL_ttf.h>
#ifdef _WIN32
#include <SDL_ttf.h>
#else
#include <SDL2_ttf/SDL_ttf.h>
#endif

#include <stdio.h>
#include <string>
//#include <cmath>



struct SDL_Renderer;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	//bool loadFromFile( std::string path );

	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor, SDL_Renderer* gRenderer, TTF_Font *gFont);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

	bool isLoaded = false;

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};




#endif /* Texture_hpp */