#pragma once

#include "gl_abstraction.h"

extern bool return_to_console;
extern bool cursor_inside;
extern bool find_value;

extern pos p;
extern double dx;
extern double dy;
extern double dz;
extern float zoom;
extern float zoom_change;

struct shadersource
{
	std::string vertex, fragment;
};

enum class shadertype
{
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1
};

//reads the shader from a file and stores it in a compilable form
shadersource parseshader(const std::string &file);

//compiles the shader
unsigned int compileshader(unsigned int type, const std::string &source);

//creates shaders to be used for the program and compiles it
unsigned int createshader(const std::string &vertexshader, const std::string &fragmentshader);

/*Initializes the glfw and glew libraries and
creates a window and defines callbacks*/
int create_window(GLFWwindow *&window, int width, int height);

//For processing keyboard key presses
void keycallback(GLFWwindow *, int, int, int, int);

//For Finding the position of the mouse cursor on the screen
void cursor_position_callback(GLFWwindow *, double, double);

//Checking whether the cursot has entered the window
void cursor_enter_callback(GLFWwindow *, int);

//For processing mouse button presses
void mouse_button_callback(GLFWwindow *, int, int, int);
