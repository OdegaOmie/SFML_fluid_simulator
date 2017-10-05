#include "stdafx.h"
#include "Fluid_Sim_2D.h"

Fluid_Sim_2D::Fluid_Sim_2D(int _w, int _h, int _scale) : width(_w), height(_h)
{

	N_x = width - 2; //Provide buffer
	N_y = height - 2;  //Provide buffer

	scale = float(_scale);
	size = width * height;
	for (int i = 0; i < size; i++)
	{
		volume_data.push_back(VolumeData());
		volume_data[i].setZero();
		volume_data_0.push_back(VolumeData());
		volume_data_0[i].setZero();
		walls.push_back(0.0f);
		divergence.push_back(0.0f);
	}

	image.create(width, height , Color::Blue);
	//image.create(25, 25, Color::Blue);
	texture.loadFromImage(image);
	sprite.setTexture(texture, true);
	sprite.setScale(scale / 5, scale / 5);
	po_pressure_color.loadFromFile("debug_assets/pressure+.jpg");
	neg_pressure_color.loadFromFile("debug_assets/pressure-.jpg");
	temperature_color.loadFromFile("debug_assets/color.jpg");
	speed_color.loadFromFile("debug_assets/speed.jpg");
	density_color.loadFromFile("debug_assets/density.jpg");


	diffusion_coefficient_pressure = 8;
	diffusion_coefficient_density = 1;
	diffusion_coefficient_velocity  = 6;
}

void Fluid_Sim_2D::update(float _dt)
{
	velocityStep(_dt);
	densityStep(_dt);
}

void Fluid_Sim_2D::reset()
{
	for (int i = 0; i < size; i++) {
		volume_data[i].setZero();
		volume_data_0[i].setZero();
		walls[i] = 0.0f;
		divergence[i] = 0.0f;
	}
}

void Fluid_Sim_2D::diffusion(float _dt, int _var, float _viscosity)
{
	float _a = _dt *  _viscosity * N_x * N_y;
	////DIFFUSION/////
		float _value;
		for (int k = 0; k < 10; k++) 
		{
			for (int i = 1; i <= N_x; i++)
			{
				for (int j = 1; j <= N_y; j++)
				{
					set0(i, j, _var, (getActual(i, j, _var)
						+ _a
						* (get0(i - 1, j, _var)
							+ get0(i + 1, j, _var)
							+ get0(i, j - 1, _var)
							+ get0(i, j + 1, _var)))
						/ (1+4*_a));
				}
			}
			applyBoundary(_var);
		}

}

void Fluid_Sim_2D::advection(float _dt, int _var)
{

	////ADVECTION/////
	int i0, j0, i1, j1;

	float x, y, z, s0, t0, s1, t1;
	float dt0X = _dt * N_x;
	float dt0Y = _dt * N_y;
	for (int i = 1; i <= N_x; i++)
	{
		for (int j = 1; j <= N_y; j++)
		{
			x = i - dt0X * getActual(i, j, 0);
			y = j - dt0Y * getActual(i, j, 1);

			if (x < 0.5) {
				x = 0.5;
			}
			else if (x > N_x + 0.5) {
				x = N_x + 0.5;
			}

			if (y < 0.5) {
				y = 0.5;
			}
			else if (y > N_y + 0.5) {
				y = N_y + 0.5;
			}
			i0 = (int)x; //cell our subsection occupied last frame
			j0 = (int)y;  //cell our subsection occupied last frame


			i1 = i0 + 1; 
			j1 = j0 + 1; 

			s1 = x - i0; 
			s0 = 1 - s1; 

			t1 = y - j0;
			t0 = 1 - t1;

			setActual(i, j, _var,
				s0
				* (t0 * get0(i0, j0, _var)
					+ t1* get0(i0, j1, _var))
				+ s1
				* (t0 * get0(i1, j0, _var)
					+ t1* get0(i1, j1, _var)));
		}
	}
	applyBoundary(_var);
}

void Fluid_Sim_2D::projection(float _dt)
{
	int i, j, k;
	
	for (i = 1; i <= N_x; i++) {
		for (j = 1; j <= N_y; j++) {
			setGradient(i, j, -0.5f*h*(getActual(i + 1, j, 0) - getActual(i - 1, j, 0) +
			getActual(i, j + 1, 1) - getActual(i, j - 1, 1)));
			setActual(i,j, 2, 0);
		}
	}
	applyBoundary(5);
	applyBoundary(2);
	for (k = 0; k<5; k++) 
	{
		for (i = 1; i <= N_x; i++) 
		{
			for (j = 1; j <= N_y; j++) 
			{
				setActual(i, j, 2, (getGradient(i, j) + getActual(i -1, j, 2) + getActual(i + 1, j, 2) +
				getActual(i, j - 1, 2) + getActual(i, j + 1, 2)) / 4);
			}
		}
	applyBoundary(2);
	}
	for (i = 1; i <= N_x; i++) 
	{
		for (j = 1; j <= N_y; j++) 
		{
			setActual(i, j, 0, 0.5*(getActual(i + 1, j, 2) - getActual(i - 1, j, 2)) / h);
			setActual(i, j, 1, 0.5*(getActual(i, j + 1, 2) - getActual(i, j - 1, 2)) / h);
		}
	}
	applyBoundary(0);
	applyBoundary(1);
}

sf::Sprite Fluid_Sim_2D::getSpriteGeneral(int var)
{
	//sprite.setScale(1, 1);
	switch (var)
	{
	default:
		break;
	case 0:
		getSpeedSprite();
		break;
	case 1:
		getPressureSprite();
		break;
	case 2:
		getDensitySprite();
		break;
	}
	//sprite.scale(scale, scale);
	return sprite;
}

void Fluid_Sim_2D::addDensity(sf::Vector2i _input)
{
	for (int i = (_input.x - (20 / scale)); i < (_input.x + (20 / scale)); i++) {
		for (int j = (_input.y - (20 / scale)); j < (_input.y + (20 / scale)); j++) {
			if (i > 0 && i < width && j > 0 && j < height) 
			{
				float d = std::sqrt(std::pow(i - _input.x, 2) + std::pow(j - _input.y, 2));
				if (d < 4 / scale) {
					setActual(i, j, 4, getActual(i, j, 4) + 5);
				}
			}
		}
	}
}

void Fluid_Sim_2D::addV(sf::Vector2i _input)
{
	for (int i = (_input.x - 20); i < (_input.x + 20); i++) {
		for (int j = (_input.y - 20); j < (_input.y + 20); j++) {
			if (i > 0 && i < width && j > 0 && j < height)
			{
				float d = std::sqrt(std::pow(i - _input.x, 2) + std::pow(j - _input.y, 2));
				if (d < 20 / 4) 
				{
					setActual(i, j, 1, getActual(i, j, 1) + 0.005f);
				}
			}
		}
	}
}

void Fluid_Sim_2D::addU(sf::Vector2i _input)
{
	for (int i = (_input.x - 20); i < (_input.x + 20); i++) {
		for (int j = (_input.y - 20); j < (_input.y + 20); j++) {
			if (i > 0 && i < width && j > 0 && j < height)
			{
				float d = std::sqrt(std::pow(i - _input.x, 2) + std::pow(j - _input.y, 2));
				if (d < 20 / 4) 
				{
					setActual(i, j, 0, getActual(i, j, 0) + 0.005f);
				}
			}
		}
	}
}

void Fluid_Sim_2D::addVelocity(sf::Vector2i _input)
{
	//for (int i = (_input.x - 8); i < (_input.x + 8); i++) {
	//	for (int j = (_input.y - 8); j < (_input.y + 8); j++) {
	//		if (i > 0 && i < width && j > 0 && j < height)
	//		{
	//			float X = i - _input.x;
	//			float Y = j - _input.y;
	//			float d = std::sqrt(std::pow(i - _input.x, 2) + std::pow(j - _input.y, 2));
	//			X /= d;
	//			Y /= d;

	//			X *= 10 / d;
	//			Y *= 10 / d;
	//			if (d < 2 /scale) 
	//			{
	//				setActual(i, j, 0, X);
	//				setActual(i, j, 1, Y);
	//			}
	//		}
	//	}
	//}
	for (int i = (_input.x - 8); i < (_input.x + 8); i++) {
		for (int j = (_input.y - 8); j < (_input.y + 8); j++) {
			if (i > 0 && i < width && j > 0 && j < height)
			{
				float d = std::sqrt(std::pow(i - _input.x, 2) + std::pow(j - _input.y, 2));
				if (d < 2 / scale)
				{
					setActual(i, j, 0, getActual(i, j, 0) + 1000000000.0f);
					setActual(i, j, 1, getActual(i, j, 1) + 1000000000.0f);
				}
			}
		}
	}
}

void Fluid_Sim_2D::applyBoundary(int var)
{
	switch (var)
	{
	default:
		break;
	case 0:
		boundaryConditionsUVelocity();
		break;
	case 1:
		boundaryConditionsVVelocity();
		break;
	case 2:
		boundaryConditionsPressure();
		break;
	case 3:
		//PlaceHOLDER
		break;
	case 4:
		boundaryConditionsDensity();
		break;
	case 5:
		boundaryConditionsDivergance();
		break;
	}
}

void Fluid_Sim_2D::boundaryConditionsUVelocity()
{
	for (int i = 1; i <= N_y; i++)
	{
		setActual(0, i, 0, -getActual(1, i, 0));
		setActual(N_x + 1, i, 0, -getActual(N_x, i, 0));
	}
	for (int i = 1; i < N_x; i++) {
		setActual(i, 0, 0, getActual(i, 1, 0));
		setActual(i, N_y + 1, 0, getActual(i, N_y, 0));
	}


	setActual(0, 0, 0, 0.5*(getActual(1, 0, 0) + getActual(0, 1, 0)));

	setActual(0, N_y + 1, 0, 0.5*(getActual(1, N_y + 1, 0) + getActual(0, N_y, 0)));

	setActual(N_x + 1, 0, 0, 0.5*(getActual(N_x, 0, 0) + getActual(N_x + 1, 1, 0)));

	setActual(N_x + 1, N_y + 1, 0, 0.5f * (getActual(N_x, N_y + 1, 0) + getActual(N_x + 1, N_y, 0)));
}

void Fluid_Sim_2D::boundaryConditionsVVelocity()
{
		int i;
		for (i = 1; i <= N_y; i++)
		{
			setActual(0, i, 1, getActual(1, i, 1));
			setActual(N_x + 1, i, 1,  getActual(N_x, i, 1));
		}
		for (int i = 1; i < N_x; i++) 
		{
			setActual(i, 0, 1,  -getActual(i, 1, 1));
			setActual(i, N_y + 1, 1,  -getActual(i, N_y, 1));
		}


		setActual(0, 0, 1, 0.5*(getActual(1, 0, 1) + getActual(0, 1, 1)));

		setActual(0, N_y + 1, 1, 0.5*(getActual(1, N_y + 1, 1) + getActual(0, N_y, 1)));

		setActual(N_x + 1, 0, 1, 0.5*(getActual(N_x, 0, 1) + getActual(N_x + 1, 1, 1)));

		setActual(N_x + 1, N_y + 1, 1, 0.5f * (getActual(N_x, N_y + 1, 1) + getActual(N_x + 1, N_y, 1)));
}

void Fluid_Sim_2D::boundaryConditionsPressure()
{
	for (int i = 1; i <= N_y; i++)
	{
		setActual(0, i, 2, -getActual(1, i, 2));
		setActual(N_x + 1, i, 2, -getActual(N_x, i, 2));
	}
	for (int i = 1; i < N_x; i++) {
		setActual(i, 0, 2, getActual(i, 1, 2));
		setActual(i, N_y + 1, 2, getActual(i, N_y, 2));
	}


	setActual(0, 0, 2, 0.5*(getActual(1, 0, 2) + getActual(0, 1, 2)));

	setActual(0, N_y + 1, 2, 0.5*(getActual(1, N_y + 1, 2) + getActual(0, N_y, 2)));

	setActual(N_x + 1, 0, 2, 0.5*(getActual(N_x, 0, 2) + getActual(N_x + 1, 1, 2)));

	setActual(N_x + 1, N_y + 1, 2, 0.5f * (getActual(N_x, N_y + 1, 2) + getActual(N_x + 1, N_y, 2)));
}

void Fluid_Sim_2D::boundaryConditionsDensity()
{
		for (int i = 1; i <= N_y; i++)
		{
			setActual(0, i, 4, getActual(1, i, 4));
			setActual(N_x + 1, i, 4, getActual(N_x, i, 4));
		}
		for (int i = 1; i < N_x; i++) {
			setActual(i, 0, 0, getActual(i, 1, 4));
			setActual(i, N_y + 1, 4, getActual(i, N_y, 4));
		}


		setActual(0, 0, 4, 0.5*(getActual(1, 0, 4) + getActual(0, 1, 4)));

		setActual(0, N_y + 1, 4, 0.5*(getActual(1, N_y + 1, 4) + getActual(0, N_y, 4)));

		setActual(N_x + 1, 0, 4, 0.5*(getActual(N_x, 0, 4) + getActual(N_x + 1, 1, 4)));

		setActual(N_x + 1, N_y + 1, 4, 0.5f * (getActual(N_x, N_y + 1, 4) + getActual(N_x + 1, N_y, 4)));
}

void Fluid_Sim_2D::boundaryConditionsDivergance()
{
	for (int i = 1; i <= N_y; i++)
	{
		setGradient(0, i, getGradient(1, i));
		setGradient(N_x + 1, i, getGradient(N_x, i));
	}
	for (int i = 1; i < N_x; i++) {
		setGradient(i, 0, getGradient(i, 1));
		setGradient(i, N_y + 1, getGradient(i, N_y));
	}


	setGradient(0, 0,  0.5*(getGradient(1, 0) + getGradient(0, 1)));

	setGradient(0, N_y + 1, 0.5*(getGradient(1, N_y + 1) + getGradient(0, N_y)));

	setGradient(N_x + 1, 0, 0.5*(getGradient(N_x, 0) + getGradient(N_x + 1, 1)));

	setGradient(N_x + 1, N_y + 1,  0.5f * (getGradient(N_x, N_y + 1) + getGradient(N_x + 1, N_y)));
}

void Fluid_Sim_2D::addSourceData(float _dt, int _var)
{
	for (int i = 0; i < width; i++) 
	{
		for (int j = 0; j < height; j++) 
		{
			set0(i, j, _var,  getActual(i, j, _var));
		}
	}

}

float Fluid_Sim_2D::getGradient(int const & _i, int const & _j) const
{
	if (_i > 0 && _i < width && _j>0 && _j < width)
	{
		return divergence[_i + (_j - 1)*width];
	}
	else return 0;
}

void Fluid_Sim_2D::setGradient(int const & _i, int const & _j, float const & _n)
{
	if (_i > 0 && _i < width && _j>0 && _j < width)
	{
		divergence[_i + (_j - 1)*width] = _n;
	}
}

float Fluid_Sim_2D::getWall(int const & i, int const & j, int const & v) const
{
	return 0.0f;
}

void Fluid_Sim_2D::setWall(int const & _i, int const & _j, int const & _v, float const & _n)
{
}

void Fluid_Sim_2D::velocityStep(float _dt)
{
	addSourceData(_dt, 0);
	addSourceData(_dt, 1);
	diffusion(_dt, 0, diffusion_coefficient_velocity);
	diffusion(_dt, 1, diffusion_coefficient_velocity);
	projection(_dt);
	advection(_dt, 0);
	advection(_dt, 1);
	projection(_dt);
}

void Fluid_Sim_2D::densityStep(float _dt)
{
	addSourceData(_dt, 4);
	diffusion(_dt, 4, diffusion_coefficient_density);
	advection(_dt, 4);
}

sf::Color Fluid_Sim_2D::colorGradient(Image &color, float x)
{
	if (x>0.999) x = 0.999;
	if (x<0.001) x = 0.001;
	return color.getPixel((int)(x*color.getSize().x), 0);
}

void Fluid_Sim_2D::getPressureSprite()
{
	for (int i = 1; i < width - 1; i++)
	{
		for (int j = 1; j < height - 1; j++)
		{
			if (getActual(i, j, 2) > 0)
			{
				image.setPixel(i, j, colorGradient(po_pressure_color, getActual(i, j, 2) / scale));
			}
			else
			{
				image.setPixel(i, j, colorGradient(neg_pressure_color, -getActual(i, j, 2) / scale));
			}
		}
	}
	texture.loadFromImage(image);
}

void Fluid_Sim_2D::getTemperatureSprite()
{
	for (int i(1); i < width - 1; i++)
		for (int j(1); j < height - 1; j++)
			image.setPixel(i, j, colorGradient(temperature_color, getActual(i, j, 3) / scale));

	texture.loadFromImage(image);
}

void Fluid_Sim_2D::getSpeedSprite()
{
	for (int i(1); i < width - 1; i++) 
	{
		for (int j(1); j < height - 1; j++) 
		{
			image.setPixel(i, j, colorGradient(speed_color, sqrt(pow(getActual(i, j, 0), 2) + pow(getActual(i, j, 1), 2)) / scale));
		}
	}
	
	texture.loadFromImage(image);
}

void Fluid_Sim_2D::getDensitySprite()
{
	for (int i(1); i < width - 1; i++)
		for (int j(1); j < height - 1; j++)
			image.setPixel(i, j, colorGradient(density_color, getActual(i, j, 4)*1.0f));

	texture.loadFromImage(image);
}

float Fluid_Sim_2D::getActual(int const& _i, int const& _j, int const& _v) const
{
	if (_i > 0 && _i < width && _j>0 && _j < width)
	{
		return volume_data[_i + (_j - 1)*width].get_var(_v);
	}
	else return 0;
}

void Fluid_Sim_2D::setActual(int const& _i, int const& _j, int const& _v, float const& _n)
{
	if (_i > 0 && _i < width && _j>0 && _j < width)
	{
		volume_data[_i + (_j - 1)*width].set_var(_n, _v);
	}
}

float Fluid_Sim_2D::get0(int const& _i, int const& _j, int const& _v) const
{
	if (_i > 0 && _i < width && _j>0 && _j < width)
	{
		return volume_data_0[_i + (_j - 1)*width].get_var(_v);
	}
	else return 0;
}

void Fluid_Sim_2D::set0(int const& _i, int const& _j, int const& _v, float const& _n)
{
	if (_i > 0 && _i < width && _j>0 && _j < width)
	{
		volume_data_0[_i + (_j - 1)*width].set_var(_n, _v);
	}
}