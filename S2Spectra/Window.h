#ifndef SDL_H
#include <SDL2/SDL.h>
#include "Spectra.h"
#endif



class LWindow
{
public:
	//Intializes internals
	LWindow();

	//Creates window
	bool init();
    
    Spectra spectra;

	//Creates renderer from internal window
	SDL_Renderer* createRenderer();

	//Handles window events
	void handleEvent(SDL_Event& e);

	//Deallocates internals
	void free();

	//Window dimensions
	int getWidth();
	int getHeight();

	//Window focii
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();
    
    //draw the content
    void draw();
    
private:
	//Window data
	SDL_Window* mWindow;
	SDL_Renderer* gRenderer;

	//Window dimensions
	int mWidth;
	int mHeight;

	//Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
    
    //if a zoom box is being drawin
    bool mTracking;
    
    //chromatogram zoom rectangle info
    int zOriginX;
    int zOriginY;
    int zWidth;
    int zHeight;
};

