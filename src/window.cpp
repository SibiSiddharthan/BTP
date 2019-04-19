#include "window.h"
#include <stdio.h>
#include <stdlib.h>


using namespace std;

window::window(int _width, int _height) : width{_width}, height{_height}
{

	p = {0, 0, 0};
	dx = 0.01;
	dy = 0.01;
	dz = 0.01;
	zoom = 1;
	zoom_change = 1.1;
	return_to_console = false;
	rotx = 0;
	roty = 0;
	rotch = 0.02;

	glfwInit();
	_window = glfwCreateWindow(width, height, "MESH", NULL, NULL);

	if (!_window)
	{
		printf("Unable to create a window");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(_window);
	glewInit();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init();
	
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(_window);
}

window::~window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(_window);
	glfwTerminate();
}

void window::event_keyboard()
{
	ImGuiIO &io = ImGui::GetIO();

	if (io.KeysDown[GLFW_KEY_W])
	{
		if (io.KeyShift)
			p.y -= 0.5 * dy;
		else if (io.KeyCtrl)
			p.y -= 2 * dy;
		else
			p.y -= dy;
	}

	if (io.KeysDown[GLFW_KEY_S])
	{
		if (io.KeyShift)
			p.y += 0.5 * dy;
		else if (io.KeyCtrl)
			p.y += 2 * dy;
		else
			p.y += dy;
	}

	if (io.KeysDown[GLFW_KEY_A])
	{
		if (io.KeyShift)
			p.x += 0.5 * dy;
		else if (io.KeyCtrl)
			p.x += 2 * dy;
		else
			p.x += dy;
	}

	if (io.KeysDown[GLFW_KEY_D])
	{
		if (io.KeyShift)
			p.x -= 0.5 * dy;
		else if (io.KeyCtrl)
			p.x -= 2 * dy;
		else
			p.x -= dy;
	}

	if (io.KeysDown[GLFW_KEY_KP_ADD])
		zoom *= zoom_change;
	if (io.KeysDown[GLFW_KEY_KP_SUBTRACT])
		zoom /= zoom_change;

	if (io.KeysDown[GLFW_KEY_UP])
	{
		rotx -= rotch;
	}

	if (io.KeysDown[GLFW_KEY_DOWN])
	{
		rotx += rotch;
	}

	if (io.KeysDown[GLFW_KEY_LEFT])
	{
		roty -= rotch;
	}

	if (io.KeysDown[GLFW_KEY_RIGHT])
	{
		roty += rotch;
	}

	if (io.KeysDown[GLFW_KEY_ESCAPE])
	{
		return_to_console = true;
		zoom = 1.0;
		p = {0, 0};
		rotx = 0;
		roty = 0;
	}
}

void window::window_refresh()
{
	ImGuiIO &io = ImGui::GetIO();
	width = io.DisplaySize.x;
	height = io.DisplaySize.y;
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
