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


int main()
{

    return 0;
}

void run_2d() {
	int sizeX = 500;
	int sizeY = 500;
	int _scale = 2;

	float _cell_width = (sizeX / (sizeX / _scale));
	float _cell_heigth = (sizeY / (sizeY / _scale));
	// Create the main window 
	sf::RenderWindow App(sf::VideoMode(sizeX, sizeY, 32), "SFML OpenGL");

	// Create a clock for measuring time elapsed     
	sf::Clock Clock;
	//create a font
	float _dt = 0.0001;

	sf::Font font;
	if (!font.loadFromFile("sansation.ttf"))
	{
		// error...
	}
	sf::Color _col, _zero_col;
	_zero_col = sf::Color(0, 0, 0, 0);
	//prepare OpenGL surface for HSR
	glClearDepth(1.f);
	glClearColor(0.3f, 0.3f, 0.3f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//// Setup a perspective projection & Camera position
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(90.f, 1.f, 1.f, 300.0f);//fov, aspect, zNear, zFar
	GLfloat zNear = 0.1f;
	GLfloat zFar = 255.0f;
	GLfloat aspect = float(800) / float(600);
	GLfloat fH = tan(float(90 / 360.0f * 3.14159f)) * zNear;
	GLfloat fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);



	bool rotate = true;
	float angle = 0;
	bool _run = false;

	int _3d_scale = 1;
	Fluid_Sim_2D _sim_2D = Fluid_Sim_2D(sizeX / _scale, sizeY / _scale, _cell_width, _scale);
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
			mousePos = sf::Vector2f(sf::Mouse::getPosition(App).x, sf::Mouse::getPosition(App).y);
			//Prepare for drawing 
			// Clear color and depth buffer 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Apply some transformations 
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();


			App.clear(sf::Color::Black);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				int X = mousePos.x / _cell_width;
				int Y = mousePos.y / _cell_heigth;
				if (X < sizeX && X > 0 && Y < sizeY && Y > 0)
				{
					_sim_2D.addDensity(X, Y);
				}
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				int X = mousePos.x / _cell_width;
				int Y = mousePos.y / _cell_heigth;
				if (X < sizeX && X > 0 && Y < sizeY && Y > 0)
				{
					_sim_2D.addVelocity(X, Y);
				}
			}
			if (_run)
			{
				_sim_2D.update(_dt);
			}
		App.draw(_sim_2D.getSpriteGeneral(_variable_shown));
		App.display();
	}
}

void run_3d() {

}