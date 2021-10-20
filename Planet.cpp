#ifndef PLANET
#define PLANET

#include <vector>
#include "olcPixelGameEngine.h"

struct Planet
{
	std::string name;
	double distance = 0.0;
	double diameter = 0.0;
	std::string athmosphere = "";
	bool life = false;
	bool ring = false;
	olc::Pixel color;
	std::vector<double> moons;
};


#endif