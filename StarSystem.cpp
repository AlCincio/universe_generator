#include "Planet.cpp"
#include "olcPixelGameEngine.h"

// define a set of colors for the stars
const olc::Pixel starColors[11] =
{
	olc::WHITE, olc::WHITE, olc::WHITE, // white giant/dwarf
	olc::CYAN, olc::BLUE, olc::DARK_BLUE, // blue giant/dwarf
	olc::RED, olc::DARK_RED, olc::YELLOW, // red giant/dwarf
	olc::VERY_DARK_RED, olc::VERY_DARK_RED // brown giant/dwarf
};

// define a set of color for the planet
const olc::Pixel planetColors[17] =
{
	// planet close to the star (burned planets/rocky planets)
	olc::RED, olc::DARK_RED, olc::DARK_YELLOW, olc::GREY, olc::MAGENTA,
	// planets in the temperate zone (planets with possible water/vegetation)
	olc::BLUE, olc::DARK_BLUE, olc::GREEN, olc::DARK_GREEN, olc::CYAN, olc::DARK_CYAN,
	// planets far from the star (frozen planets/ rocky planets / gas planets)
	olc::WHITE, olc::GREY, olc::VERY_DARK_BLUE, olc::VERY_DARK_CYAN, olc::VERY_DARK_YELLOW, olc::DARK_GREY
};

// define a set of possible athmospheres
const std::string athmosphereType[6]{
	"None", "CO2", "Oxygen", "Helium", "Hydrogen", "Methane"
};

class StarSystem
{

public:

	// set the state of the star system. whether it exists or not
	bool starExists = false;
	// set the diameter of the star
	double starDiameter = 0.0f;
	// set type of star
	std::string starType = "";
	// set system name
	std::string systemName = "";
	// set the color of the star using the olcPixelGameEngine library
	// 32-Bit RGBA colour
	olc::Pixel starColor = olc::WHITE;
	// set the vector of planets of the star system
	std::vector<Planet> planets;

	// x and y are the location of the star system in the universe
	// the boolean starSelected is used to check if we need to
	// generate more info for a particular system
	StarSystem(uint32_t x, uint32_t y, bool starSelected = false)
	{
		// Set seed based on location of star system
		// this approach garantees that the seed is different
		// for each pixel in the screen, so for each location 
		// we have the same sequence of random numbers
		lehmerSeed = (x & 0xFFFF) << 16 | (y & 0xFFFF);

		// set true with a probability of 1/20
		// this avoids filling up the universe with stars
		starExists = (randomInt(0, 20) == 1);
		// abort the construction of the system if it doesnt exists
		if (!starExists) return;

		// use random numbers to select star color from the set
		starColor = starColors[randomInt(0, 10)];
		// Use random numbers to generate star diameter
		starDiameter = randomInt(10, 60);
		// define system name
		systemName = randomName(x+y);
		// use color and diameter to construct
		// the star type
		if (starColor == olc::WHITE) {
			starType.append("White");
		}
		else if (starColor == olc::CYAN ||
			starColor == olc::BLUE ||
			starColor == olc::DARK_BLUE) {
			starType.append("Blue");
		}
		else if (starColor == olc::RED ||
			starColor == olc::DARK_RED ||
			starColor == olc::YELLOW) {
			starType.append("Red");
		}
		else if (starColor == olc::VERY_DARK_RED) {
			starType.append("Brown");
		}

		if (starDiameter > 50.0) {
			starType = "Supergiant";
		}
		else if (starDiameter > 30.0) {
			starType.append(" Giant");
		}
		else {
			starType.append(" Dwarf");
		};
		

		// if we are not in the system view, then return
		// otherwise continue generate info
		if (!starSelected) return;

		// Generate Planets
		double distanceFromStar = 50;
		int numberOfPlanets = randomInt(0, 6);
		for (int i = 0; i < numberOfPlanets; i++)
		{
			Planet p;
			p.name = randomName(x + y + i+1);
			p.distance = distanceFromStar * randomInt(1, 4);
			p.diameter = randomDouble(4.0, 20.0);
			p.athmosphere = athmosphereType[randomInt(0, 6)];
			
			if (p.distance == 50) {
				p.color = planetColors[randomInt(0, 4)];
				p.life = randomInt(0, 20) == 1;
			}
			else if (p.distance == 100) {
				p.color = planetColors[randomInt(5, 10)];
				p.life = randomInt(0, 5) == 1;
			}
			else if (p.distance == 150) {
				p.color = planetColors[randomInt(11, 16)];
				p.life = randomInt(0, 20) == 1;
			}

			// 10% of planets have a ring
			p.ring = randomInt(0, 10) == 1;

			// Satellites (Moons)
			int moonsNumber = randomInt(0, 5);
			for (int n = 0; n < moonsNumber; n++)
			{
				p.moons.push_back(randomDouble(1.0, 5.0));
			}

			// Add planet to vector
			planets.push_back(p);
		}
	}

	~StarSystem()
	{

	}



private:
	uint32_t lehmerSeed = 10;

	// generate a random double between min and max
	double randomDouble(double min, double max)
	{
		return ((double)lehmerRandomNumber() / (double)(0x7FFFFFFF)) * (max - min) + min;
	}

	// generate a random int between min and max
	int randomInt(int min, int max)
	{
		return (lehmerRandomNumber() % (max - min)) + min;
	}

	// Modified from this for 64-bit systems:
	// https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/
	uint32_t lehmerRandomNumber()
	{
		lehmerSeed += 0xe120fc15;
		uint64_t tmp;
		tmp = (uint64_t)lehmerSeed * 0x4a39b70d;
		uint32_t m1 = (tmp >> 32) ^ tmp;
		tmp = (uint64_t)m1 * 0x12fad5c9;
		uint32_t m2 = (tmp >> 32) ^ tmp;
		return m2;
	}

	// Random name generator function
	std::string randomName(int seed) {
		// set a seed based on the position of the star/planets
		srand(seed);
		// set a random length between 2 and 12
		int length = rand() % 10 + 2;
		// define vowels and consonants
		char consonants[] = { 'B','C','D','F','G','H','J','K','L','M','N','P','Q','R','S','T','V','Z','X','W' };
		char vowels[] = { 'A','E','I','O','U','Y'};

		std::string name = "";

		// set a random boolean (0 or 1)
		int random = rand() % 2;
		int consCount = 0;
		int vowCount = 0;

		for (int i = 0; i < length; i++) {
			// if random is 0, choose a consonant
			// unless there are already 2 in a row
			if ((random < 1 && consCount < 2) || vowCount == 2) {
				name += consonants[rand() % 20];
				consCount++;
				vowCount = 0;
			}
			// if random is 1 choose a vowel unless there are already 2 in a row
			else {
				name = name + vowels[rand() % 6];
				vowCount++;
				consCount = 0;
			}
			// reset random number
			random = rand() % 2;

		}
		return name;
	}
};