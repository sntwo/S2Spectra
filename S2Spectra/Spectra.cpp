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

void Spectra::render (SDL_Renderer* renderer, float screenWidth, float screenHeight){
    float h = screenHeight - 50;
    xFactor = screenWidth / (maxX * 1.1);
    yFactor = screenHeight / ((maxY - minY) * 1.5f); //pad out the y dimension a bit
    
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    for (int i = 0; i < count; i++){
        float x1 = times[i] * xFactor + 20;
        float y1 = h - intensities[i] * yFactor;
        float x2 = times[i + 1] * xFactor + 20;
        float y2 = h - intensities[i + 1] * yFactor;
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

float Spectra::time(int x){
    return (x - 20) / xFactor;
}
float Spectra::intensity(int y, int screenHeight){
    float h = screenHeight - 50;
    return -(y - h) / yFactor;
}

Spectra::Spectra(std::string fileName) {
    
    string line;
    ifstream myfile(fileName);
    if (myfile.is_open()){
        
        getline(myfile, line);  // chromatogram
        cout << line << "\n";
        getline(myfile, line);  // name
        cout << line << "\n";
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
        cout << "maxY is " << maxY << "\n";
    }
}

