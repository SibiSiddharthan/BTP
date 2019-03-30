#ifndef WINDOW_H
#define WINDOW_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "position.h"
#include "draw_object.h"

class window
{
  private:
	GLFWwindow *_window;
	/*
	static pos p;
	static float dx,dy,dz;
	static float zoom, zoom_change;

	static float rotx, roty, rotch;
	static bool return_to_console;
	*/
  public:
	friend void event_keyboard(GLFWwindow *__window, int key, int scancode, int action, int mods) ;
	window(int width = 800, int height = 800);

	/*
	inline pos get_pos()
	{
		return p;
	}

	inline float get_zoom()
	{
		return zoom;
	}
	*/

	inline void swap_buffers()
	{
		glfwSwapBuffers(_window);
	}

	inline void poll_events()
	{
		glfwPollEvents();
	}




	void draw(const std::string& shaders) const;
	
};

#endif