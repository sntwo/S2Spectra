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
#include <algorithm>


using namespace std;

Spectra::Spectra(){}

float xOffset = 0;
float yOffset = 0;


int Spectra::getTimeIndexForTime(float t){

	float offset = maxX + 1;
	//std::cout << "offset is " << offset << "\n";
	for (int i = 0; i < count; i++){
		float diff = fabs(times[i] - t);
		//std::cout << "diff is " << diff << "\n";
		if (diff < offset) {
			offset = diff;
		}
		else {
			return i;
		}
	}

	return 0;
}

void Spectra::deleteRange(float startTime, float endTime) {
	for (int i = 0; i < 50; i++){
		if (integrations[i].isSet){
			float st = times[integrations[i].startIndex];
			float et = times[integrations[i].endIndex];
			if (st < endTime && st > startTime) { integrations[i].isSet = false; }
			if (et < endTime && st > startTime) { integrations[i].isSet = false; }
		}
	}

}

bool ValueCmp(Integration const & a, Integration const & b)
{
	return a.time < b.time;
}

int Spectra::integrate(float startTime, float endTime) {
	//printf("integrating");
	deleteRange(startTime, endTime);
	//find first available integration
	int emptyIndex;
	for (emptyIndex = 0; emptyIndex < 50; emptyIndex++){
		if (!integrations[emptyIndex].isSet) {
			break;
		}
	}
	int startIndex = getTimeIndexForTime(startTime);
	int endIndex = getTimeIndexForTime(endTime);
	integrations[emptyIndex].startIndex = startIndex;
	integrations[emptyIndex].endIndex = endIndex;

	/*std::cout << "integration index is " << emptyIndex << "\n";
	std::cout << "set integration start time to " << times[integrations[emptyIndex].startIndex] << "\n";
	std::cout << "set integration end time to " << times[integrations[emptyIndex].endIndex] << "\n";*/

	//only count the integration if it is longer than one second
	if (times[endIndex] - times[startIndex] > 0.016){
		integrations[emptyIndex].isSet = true;
	}

	float rise = intensities[endIndex] - intensities[startIndex];
	float run = times[endIndex] - times[startIndex];
	float slope = rise / run;

	float area = 0;
	int runningHeightIndex = 0;
	float runningHeight = 0;
	for (int i = startIndex; i < endIndex + 1; i++){
		if (intensities[i] > runningHeight) { runningHeight = intensities[i]; runningHeightIndex = i; }
		float ct = intensities[i] - ( slope * ( times[i] - times[startIndex]) + intensities[startIndex] );
		if (ct > 0) {
			area += ct;
		}
	}
	integrations[emptyIndex].area = area;
	integrations[emptyIndex].time = times[runningHeightIndex];
	//std::cout << "area is " << area << "\n";

	std::sort(integrations, integrations + 50, ValueCmp);

	redoStrings();

	return emptyIndex;
}

void Spectra::redoStrings()
{
	long totalCount = 0;
	for (int i = 0; i < 50; i++){
		if (integrations[i].isSet){
			totalCount += integrations[i].area;
		}
	}

	for (int i = 0; i < 50; i++){
		if (integrations[i].isSet){
			integrations[i].labelString = "";
			integrations[i].labelString += std::to_string(100 * integrations[i].area / totalCount);
		}
	}
}

void Spectra::loadFromString(string str) {

	std::istringstream myfile(str);
	std::string line;

	getline(myfile, line);  // chromatogram
	//cout << line << "\n";
	getline(myfile, name);  // name
	cout << "Loading " << name << "\n";
	getline(myfile, line);  // data points
	cout << "Found " << line << "data points \n";
	istringstream dp(line);
	string skip;

	dp >> skip >> skip >> count;
	//cout << "count: " << count << "\n";
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
	isLoaded = true;

	/*cout << "maxX is " << maxX << "\n";
	cout << "minX is " << minX << "\n";
	cout << "maxY is " << maxY << "\n";
	cout << "minY is " << minY << "\n";*/
}

	



Spectra::Spectra(std::string fileName) {

	string line;
	ifstream myfile(fileName);
	if (myfile.is_open()){

		getline(myfile, line);  // chromatogram
		//cout << line << "\n";
		getline(myfile, name);  // name
		cout << "Loading " << name << "\n";
		getline(myfile, line);  // data points
		cout << "Found " << line << "data points \n";
		istringstream dp(line);
		string skip;

		dp >> skip >> skip >> count;
		//cout << "count: " << count << "\n";
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
		
		/*cout << "maxX is " << maxX << "\n";
		cout << "minX is " << minX << "\n";
		cout << "maxY is " << maxY << "\n";
		cout << "minY is " << minY << "\n";*/
	}

	isLoaded = true;
}
