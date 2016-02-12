#ifndef SDL_H

#include "Spectra.h"
#endif

union SDL_Event;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;

class LWindow
{
public:
	//Intializes internals
	LWindow();

	//Creates window
	bool init();

	Spectra spectra;

	
	
	Spectra spectras[6];

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
	
	void setColor(int i);

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
	bool isZoomed;

	//chromatogram zoom rectangle info
	int zOriginX;
	int zOriginY;
	int zWidth;
	int zHeight;
	float xOffset;
	float yOffset;

	void render(int);
	float timeStart;
	float timeEnd;
	float minIntensity;
	float maxIntensity;

	float time(int);
	float intensity(int);

	float xFactor;
	float yFactor;
};
