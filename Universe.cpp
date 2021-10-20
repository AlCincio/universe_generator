#define OLC_PGE_APPLICATION
#define PI 3.14159265

#include <random>
#include "olcPixelGameEngine.h"
#include "Planet.cpp"
#include "StarSystem.cpp"


class Universe : public olc::PixelGameEngine
{
public:
	Universe()
	{
		sAppName = "Universe";
	}

	bool OnUserCreate() override
	{
		return true;
	}

	// set a 2 dimentional vector of floats (defined in olcPixelGameEngine)
	// that hold the coordinate of the position of the camera in the universe
	olc::vf2d cameraPosition = { 0,0 };
	bool selected = false;
	uint32_t system_x_coord = 0;
	uint32_t system_y_coord = 0;

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (fElapsedTime <= 0.0001f) return true;

		// set the keyboard controls to move the camera around with W A S D
		// fElapsedTime from olcPixelGameEngine allows to adjust the movement
		// according to the framerate
		if (GetKey(olc::W).bHeld) cameraPosition.y -= 50.0f * fElapsedTime;
		if (GetKey(olc::S).bHeld) cameraPosition.y += 50.0f * fElapsedTime;
		if (GetKey(olc::A).bHeld) cameraPosition.x -= 50.0f * fElapsedTime;
		if (GetKey(olc::D).bHeld) cameraPosition.x += 50.0f * fElapsedTime;
		
		// clear the screen
		Clear(olc::BLACK);
		
		// set how many star sectors there are in the screen
		int sectors_x = ScreenWidth() / 16;
		int sectors_y = ScreenHeight() / 16;

		// set a vector to store the coordinates of the mouse
		// divided for the number of sectors, so we know which sector
		// the mouse is in (we use Getmouse from olcPixelGameEngine)
		olc::vi2d mousePosition = { GetMouseX() / 16, GetMouseY() / 16 };

		// set a 2 dimentional vector of integers (defined in olcPixelGameEngine)
		// that hold the coordinate of a sector on the screen
		olc::vi2d screen_sector = { 0,0 };

		// scan through all the sectors in the screen
		for (screen_sector.x = 0; screen_sector.x < sectors_x; screen_sector.x++)
			for (screen_sector.y = 0; screen_sector.y < sectors_y; screen_sector.y++)
			{
				uint32_t seed1 = (uint32_t)cameraPosition.x + (uint32_t)screen_sector.x;
				uint32_t seed2 = (uint32_t)cameraPosition.y + (uint32_t)screen_sector.y;

				StarSystem star(seed1, seed2);

				// check if star system exists and in that case, draw it
				if (star.starExists)
				{
					// draw a circle based on the size of the star
					FillCircle(
						screen_sector.x * 16 + 8, 
						screen_sector.y * 16 + 8,
						(int)star.starDiameter / 8, 
						star.starColor);

					// Highlight the star system when the mouse is over it
					if (mousePosition.x == screen_sector.x && mousePosition.y == screen_sector.y)
					{
						DrawCircle(
							screen_sector.x * 16 + 8, 
							screen_sector.y * 16 + 8, 
							12, 
							olc::YELLOW);
					}
				}
			}

		// handle the mouse click in the universe and 
		// check if there is a system in that position
		if (GetMouse(0).bPressed)
		{
			// get mouse position
			uint32_t mouse_x = (uint32_t)cameraPosition.x + (uint32_t)mousePosition.x;
			uint32_t mouse_y = (uint32_t)cameraPosition.y + (uint32_t)mousePosition.y;

			// get star system in that position
			StarSystem star(mouse_x, mouse_y);

			// check if there is an active system
			if (star.starExists)
			{
				selected = true;
				system_x_coord = mouse_x;
				system_y_coord = mouse_y;
			}
			else
				selected = false;
		}

		// if the star system is selected,
		// initiate the drawing routine and show
		// all the details of the system
		if (selected)
		{
			// Generate full star system
			// with planets, moons, etc
			StarSystem star(system_x_coord, system_y_coord, true);

			// Draw Window
			FillRect(192, 60, 476, 360, olc::BLACK);
			DrawRect(192, 60, 476, 360, olc::WHITE);

			// Draw Star
			// set the star position
			olc::vi2d starDisplayPosition = {424, 240};

			FillCircle(starDisplayPosition, (int)(star.starDiameter/2.5), star.starColor);
			DrawCircle(starDisplayPosition, 50, olc::VERY_DARK_GREY);
			DrawCircle(starDisplayPosition, 100, olc::VERY_DARK_GREY);
			DrawCircle(starDisplayPosition, 150, olc::VERY_DARK_GREY);

			// number of planets in the system
			size_t numberOfPlanets = star.planets.size();
			// calculate angle in order to separate the planets
			// around the star and avoid overlap
			double standardPlanetAngle = 360;
			if (numberOfPlanets != 0)
				standardPlanetAngle = standardPlanetAngle / numberOfPlanets;
			double planetAngle = standardPlanetAngle;
			// Draw Planets
			for (auto& planet : star.planets)
			{
				// set initial position in the center of the system
				olc::vi2d planetDisplayPosition = starDisplayPosition;
				// use trigonometry to reach the position of the planet
				// based on the angle
				planetDisplayPosition.x += planet.distance * sin(planetAngle * PI / 180);
				planetDisplayPosition.y += planet.distance * cos(planetAngle * PI / 180);
				// rotate for next planet
				planetAngle += standardPlanetAngle;
				// draw planet
				FillCircle(planetDisplayPosition, (int)(planet.diameter/2), planet.color);

				if (planet.moons.size() > 0) {
					DrawCircle(planetDisplayPosition, 25, olc::VERY_DARK_GREY);
				}
				
				// draw ring
				if (planet.ring) {
					DrawLine(
						planetDisplayPosition.x - planet.diameter/1.5,
						planetDisplayPosition.y - planet.diameter/1.5,
						planetDisplayPosition.x + planet.diameter/1.5,
						planetDisplayPosition.y + planet.diameter/1.5
						);
				}

				// get number of moons of the planet
				size_t numberOfMoons = planet.moons.size();
				// calculate angle in order to separate the moons
				// around the planet and avoid overlap
				double standardMoonAngle = 360;
				if (numberOfMoons != 0)
					standardMoonAngle = standardMoonAngle / numberOfMoons;
				double moonAngle = standardMoonAngle;
				
				// Draw Moons
				for (auto& moon : planet.moons)
				{
					olc::vi2d moonDisplayPosition = planetDisplayPosition;
					
					// use trigonometry to ge the position of the moon
					moonDisplayPosition.x += 25 * sin(moonAngle*PI/180);
					moonDisplayPosition.y += 25 * cos(moonAngle*PI/180);
					// draw the moon
					FillCircle(moonDisplayPosition, (int)(moon/2), olc::GREY);
					// rotate for next moon
					moonAngle += standardMoonAngle;
					
				}

				
			}


			// Draw information window
			FillRect(5, 5, 180, 470, olc::BLACK);
			DrawRect(5, 5, 180, 470, olc::WHITE);

			// Get the information of the system and draw them in the window
			DrawString(7, 10, "System: " + star.systemName, olc::WHITE, 1);
			DrawString(7, 20, "Star Type: " + star.starType, olc::WHITE, 1);
			DrawString(7, 30, "Planets: " + std::to_string(star.planets.size()), olc::WHITE, 1);
			int h = 50;

			// loop throught the planets and get the informations
			// and fraw them in the window
			for (auto& planet : star.planets) {
				DrawString(7, h, "Planet: " + planet.name, olc::WHITE, 1);
				h += 10;
				DrawString(7, h, "Diameter km: " + std::to_string(planet.diameter), olc::WHITE, 1);
				h += 10;

				std::string d;
				if (planet.distance == 50) {
					d = "Too close";
				}
				else if (planet.distance == 100) {
					d = "Optimal";
				}
				else if (planet.distance == 150) {
					d = "Too far";
				}
				DrawString(7, h, "Distance: " + d, olc::WHITE, 1);
				h += 10;
				DrawString(7, h, "Athmosphere: " + planet.athmosphere, olc::WHITE, 1);
				h += 10;
				if (planet.life) {
					DrawString(7, h, "Has Life: Yes", olc::WHITE, 1);
				}
				else {
					DrawString(7, h, "Has Life: No", olc::WHITE, 1);
				}
				h += 10;
				DrawString(7, h, "Moons: " + std::to_string(planet.moons.size()), olc::WHITE, 1);
				
				h += 20;
			}
		}

		return true;
	}
};

int main()
{
	Universe uni;
	if (uni.Construct(720, 480, 2, 2, false, false))
		uni.Start();
	return 0;
}