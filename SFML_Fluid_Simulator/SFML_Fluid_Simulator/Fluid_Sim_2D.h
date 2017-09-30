#pragma once
#include "SFML/Graphics.hpp" 
#include "SFML/OpenGL.hpp" 
#include <vector>
#include <iostream> 
#include <algorithm>

using namespace sf;

class VolumeData 
{	
	// 0 = x_velocity
	// 1 = y_velocity
	// 2 = pressure
	// 3 = Temperatire
	// 4 = density
public:
	float* _v_data = new float[5];
	VolumeData() {
		setZero();
	}
	void setZero() {
		std::fill(_v_data, _v_data + 5, 0);
	}
	void set_var(float _new_value, int _var_index ) {
		_v_data[_var_index] = _new_value;
	}
	void increment_var(float _new_value, int _var_index) {
		_v_data[_var_index] += _new_value;
	}
	void decrement_var(float _new_value, int _var_index) {
		_v_data[_var_index] -= _new_value;
	}
	float get_var(int _index) { 
		return _v_data[_index];
	}
	float get_var(int _index) const {
		return _v_data[_index];
	}
};




class Fluid_Sim_2D
{

private:

	int width, height;

	float size;

	int N_x;
	int N_y;


	std::vector<VolumeData> volume_data;
	std::vector<VolumeData> volume_data_0;
	std::vector<float> walls;
	std::vector<float> divergence;
	// 0 = u
	// 1 = v
	// 2 = p
	// 3 = T
	// 4 = d



	sf::Image po_pressure_color;
	sf::Image neg_pressure_color;
	sf::Image temperature_color;
	sf::Image speed_color;
	sf::Image density_color;

	Image image;
	Texture texture;
	Sprite sprite;
	int scale;

	bool pressure_added_this_frame;

	float diffusion_coefficient_pressure;
	float diffusion_coefficient_density;
	float diffusion_coefficient_velocity;

public:
	Fluid_Sim_2D(int width, int height, int size, int scale);


	void addDensity(int x, int y);
	void addPressure(int x, int y);
	void addV(int x, int y);
	void addU(int x, int y);
	void addVelocity(int x, int y);


	sf::Color colorGradient(sf::Image &color, float x);

	void applyBoundary(int var);// extra for divergance

	void boundaryConditionsDensity();
	void boundaryConditionsDivergance();
	void boundaryConditionsPressure();
	void boundaryConditionsUVelocity();
	void boundaryConditionsVVelocity();

	void addSourceData(float _dt, int var);

	float getActual(int const& i, int const& j, int const& v) const;
	void setActual(int const& i, int const& j, int const& v, float const& n);

	float get0(int const& i, int const& j, int const& v) const;
	void set0(int const& i, int const& j, int const& v, float const& n);

	float getGradient(int const& i, int const& j) const;
	void setGradient(int const& i, int const& j, float const& n);

	float getWall(int const& i, int const& j, int const& v) const;
	void setWall(int const& i, int const& j, int const& v, float const& n);


	sf::Sprite getPressureSprite();
	sf::Sprite getTemperatureSprite();
	sf::Sprite getSpeedSprite();
	sf::Sprite getDensitySprite();

	sf::Sprite getSpriteGeneral(int var);



	void diffusion(float _dt, int v,float viscosity);
	void advection(float _dt, int v);
	void projection(float _dt);
	void densityStep(float _dt);
	void velocityStep(float _dt);

	void update(float _dt);
};
