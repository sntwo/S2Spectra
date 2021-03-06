#ifndef SDL_H

#include "Spectra.h"
#include "Texture.h"

union SDL_Event;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;


enum DrawMode { zoom, integrate, deintegrate };

class LWindow
{
public:
	//Intializes internals
	LWindow();

	//Creates window
	bool init();

	DrawMode drawMode;

	Spectra spectras[6];
	LTexture nameTextures[6];
	

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

	//mouse position;
	int mx = 0;
	int my = 0;

private:
	//Window data
	SDL_Window* mWindow;
	SDL_Renderer* gRenderer;

	void setColor(int i);
	SDL_Color getColor(int i);

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
    int xSmoother;

	//checks to see if click is in name box region, returning index of spectra if true or -1 if false
	int checkNameBoxes(int x, int y);

	void makeKey(int i);

	LTexture timeLabel;
	LTexture areaLabel;
	LTexture areaPercentLabel;
    
    void redoStrings();
};




#endif
