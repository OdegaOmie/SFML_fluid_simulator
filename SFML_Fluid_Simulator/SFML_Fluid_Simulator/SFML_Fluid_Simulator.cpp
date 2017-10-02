// SFML_Fluid_Simulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 

#include <iostream> 
#include "SFML/Graphics.hpp" 
#include "SFML/OpenGL.hpp"

#include "Fluid_Sim_2D.h"



sf::Vector2i getCorinates2D(sf::Vector2f _input, float _cell_width, float _cell_heigth) {
	int X = _input.x / _cell_width;
	int Y = _input.y / _cell_heigth;
	return sf::Vector2i(X, Y);
}


void run_2d() {
	int sizeX = 500;
	int sizeY = 500;
	int scale = 2;
	bool running = true;

	float cell_width = (sizeX / (sizeX / scale));
	float cell_heigth = (sizeY / (sizeY / scale));
	// Create the main window 
	sf::RenderWindow App(sf::VideoMode(sizeX, sizeY, 32), "SFML OpenGL");

	// Create a clock for measuring time elapsed     
	sf::Clock Clock;
	//create a font
	float dt = 0.00001;

	int variable_shown = 0;

	Fluid_Sim_2D sim_2D = Fluid_Sim_2D(sizeX / scale, sizeY / scale, cell_width, scale);
	sf::Vector2f mousePos;

	while (App.isOpen())
	{
		// Process events 
		sf::Event Event;
		while (App.pollEvent(Event))
		{
			// Close window : exit 
			if (Event.type == sf::Event::Closed)
				App.close();

			// Escape key : exit 
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
				App.close();
		}
		mousePos = Vector2f(Mouse::getPosition(App).x, Mouse::getPosition(App).y);

		bool RMB = Mouse::isButtonPressed(Mouse::Right);
		bool LMB = Mouse::isButtonPressed(Mouse::Left);
		bool R = Keyboard::isKeyPressed(Keyboard::R);
		bool speed = Keyboard::isKeyPressed(Keyboard::Num1);
		bool pressure = Keyboard::isKeyPressed(Keyboard::Num2);
		bool density = Keyboard::isKeyPressed(Keyboard::Num3);

		if (speed) {
			variable_shown = 0;
		}
		else if (pressure) {
			variable_shown = 1;
		}
		else if (density) {
			variable_shown = 2;
		}


		if (LMB) sim_2D.addDensity(getCorinates2D(mousePos, cell_width, cell_heigth));
		if (RMB)sim_2D.addPressure(getCorinates2D(mousePos, cell_width, cell_heigth));
		if (R) sim_2D.reset();



		if (running)
		{
			sim_2D.update(dt);
		}
		App.draw(sim_2D.getSpriteGeneral(variable_shown));
		App.display();
		std::cout << "LOOP" << std::endl;
	}
}

void run_3d() {

}


int main()
{
	run_2d();
	return 0;
}