#pragma once
#include "mesh_basic.h"
#include "display.h"

class _3D_
{
	private:
	std::vector<node> N;
	//std
	std::vector<plane> P;
	GLFWwindow *window = nullptr;

	public:
	inline void attach_window(GLFWwindow *_window)
	{
		window = _window;
	}

	void display();

	void stl_read(const std::string filepath);


};