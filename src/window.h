#ifndef WINDOW_H
#define WINDOW_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "position.h"
//#include "draw_object.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class window
{
  private:
	GLFWwindow *_window;
	int width,height;
	
	pos p;
	float dx,dy,dz;
	float zoom, zoom_change;

	float rotx, roty, rotch;
	bool return_to_console;
	

  public:
	window(int _width = 800, int _height = 800);
	~window();
	void event_keyboard();
	void window_refresh();
	
	inline pos get_pos()
	{
		return p;
	}

	inline float get_zoom()
	{
		return zoom;
	}
	
	inline int get_width()
	{
		return float(width);
	}

	inline int get_height()
	{
		return float(height);
	}

	inline void swap_buffers()
	{
		glfwSwapBuffers(_window);
	}

	inline void poll_events()
	{
		glfwPollEvents();
		event_keyboard();
	}

	inline bool should_close()
	{
		return glfwWindowShouldClose(_window);
	}

//	void draw(const std::string& shaders) const;
	
};

#endif