#pragma once

#include <glad\glad.h>
#include "SOIL2/SOIL2.h"

#include <functional>
#include <vector>

class Utils
{
public:
	
	/**	void LoadTexturePNG(int * inTexID, const char * path)
	*
	*	Parameters:
	*	- int * inTexID: Pointer to where the texture ID is to be
	*	stored.
	*	- const char * path: File path to the PNG file (as c-string)
	*
	*	Use:
	*	This function can be used to load a PNG file into an OpenGL
	*	texture.
	*/
	static void LoadTextureFromImage(GLuint * inTexID, const char * path);


	/** bool InitFBO(GLuint * fb, GLint * texId, GLsizei width, GLsizei height, GLint format)
	*
	*   Use: Generate Framebuffer Object.
	*/
	static bool GenerateDepthFBO(GLuint * fb, GLuint * texId, GLsizei width, GLsizei height);


	template <typename StateType>
	static StateType RungeKutta2(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt);


	template <typename StateType>
	static std::vector<StateType> RungeKutta2(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt, int Nsteps);


	template <typename StateType>
	static StateType RungeKutta4(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt);
	

	template <typename StateType>
	static std::vector<StateType> RungeKutta4(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt, int Nsteps);
};

template<typename StateType>
inline StateType Utils::RungeKutta2(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt)
{
	StateType k1, k2;

	k1 = dt * dxdt(x0, t0);
	k2 = dt * dxdt(x0 + k1 / 2, t0 + dt / 2);

	return x0 + k2;
}

template<typename StateType>
inline std::vector<StateType> Utils::RungeKutta2(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt, int Nsteps)
{
	std::vector<StateType> res(Nsteps);

	for (int i = 0; i < Nsteps, i++)
		res[i] = RungeKutta2(x0, t0 + i * dt, dt, dxdt);

	return res;
}

template<typename StateType>
inline StateType Utils::RungeKutta4(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt)
{
	StateType k1, k2, k3, k4;

	k1 = dt * dxdt(x0, t0);
	k2 = dt * dxdt(x0 + k1 / 2, t0 + dt / 2);
	k3 = dt * dxdt(x0 + k2 / 2, t0 + dt / 2);
	k4 = dt * dxdt(x0 + k3, t0 + dt);

	return x0 + (k1 + 2 * k2 + 2 * k3 + k4)/6;
}

template<typename StateType>
inline std::vector<StateType> Utils::RungeKutta4(StateType x0, double t0, double dt, std::function<StateType(StateType x, double t)> dxdt, int Nsteps)
{
	std::vector<StateType> res(Nsteps);

	for (int i = 0; i < Nsteps, i++)
		res[i] = RungeKutta4(x0, t0 + i * dt, dt, dxdt);

	return res;
}
