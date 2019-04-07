#include "window.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

window::window(int _width, int _height):width{_width},height{_height}
{
	/*
	p = {0,0,0};
	dx = 0.1;
	dy = 0.1;
	dz = 0.1;
	zoom = 1;
	zoom_change = 1.1;
	return_to_console = false;
	*/

	glfwInit();
	_window = glfwCreateWindow(width, height, "MESH", NULL, NULL);

	if (!_window)
	{
		printf("Unable to create a window");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(_window);
	glewInit();
	//Events
	//glfwSetKeyCallback(_window, event_keyboard);
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	//glfwSetCursorPosCallback(window, cursor_position_callback);
	//glfwSetCursorEnterCallback(window, cursor_enter_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	//glfwSetWindowRefreshCallback(window, window_refresh_callback);

	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(_window);
	
}

/*
void event_keyboard(GLFWwindow *__window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_W:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				window::p.y -= 0.5 * window::dy;
			else if (mods == GLFW_MOD_CONTROL)
				window::p.y -= 2 * window::dy;
			else
				window::p.y -= window::dy;
		}
	}
	break;

	case GLFW_KEY_S:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				window::p.y += 0.5 * window::dy;
			else if (mods == GLFW_MOD_CONTROL)
				window::p.y += 2 * window::dy;
			else
				window::p.y += window::dy;
		}
	}
	break;

	case GLFW_KEY_A:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				window::p.x += 0.5 * window::dx;
			else if (mods == GLFW_MOD_CONTROL)
				window::p.x += 2 * window::dx;
			else
				window::p.x += window::dx;
		}
	}
	break;

	case GLFW_KEY_D:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				window::p.x -= 0.5 * window::dx;
			else if (mods == GLFW_MOD_CONTROL)
				window::p.x -= 2 * window::dx;
			else
				window::p.x -= window::dx;
		}
	}
	break;

	case GLFW_KEY_KP_ADD:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			window::zoom *= window::zoom_change;
		}
	}
	break;

	case GLFW_KEY_KP_SUBTRACT:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			window::zoom /= window::zoom_change;
		}
	}
	break;

	case GLFW_KEY_UP:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			window::rotx -= window::rotch;
		}
	}
	break;

	case GLFW_KEY_DOWN:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			window::rotx += window::rotch;
		}
	}
	break;

	case GLFW_KEY_LEFT:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			window::roty -= window::rotch;
		}
	}
	break;

	case GLFW_KEY_RIGHT:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			window::roty += window::rotch;
		}
	}
	break;

	case GLFW_KEY_ESCAPE:
	{
		if (action == GLFW_PRESS)
		{
			window::return_to_console = true;
			window::zoom = 1.0;
			window::p = {0, 0};
			window::rotx = 0;
			window::roty = 0;
		}
	}
	break;
	}
}

/*
void window::draw(draw_object dobj,const string &source)
{
	program p(source);
	dobj.draw_objects();
	swap_buffers();
	poll_events();
}
*/
