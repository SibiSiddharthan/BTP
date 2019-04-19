#ifndef WINDOW_H
#define WINDOW_H
/**
 * @file window.h
 * @brief Implements the window creation wrapper for GLFW
 * and GUI wrapper for ImGui
 * @date 2019-04-17
 * 
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "position.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

/**
 * @class window
 * @brief Abstraction for creating ,destroying
 * a window and handling events
 * 
 * 
 * @var window::zoom Amount by which the camera is zoomed in  
 * @var window::zoom Amount by which the camera zoom level is changed
 * 
 * @var window::rotx Amount by which the object is rotated along x-axis  
 * @var window::roty Amount by which the object is rotated along y-axis  
 * @var window::rotch Amount by which the rotation of the object is changed  
 */
class window
{
  private:
	/**
    * @brief The window pointer
    */
	GLFWwindow *_window;
	/**
	 * @brief height of the window
	 */
	int width,
		/**
	 * @brief width of the window
	 * 
	 */
		height;

	/**
	 * @brief Postion of the camera
	 */
	pos p;

	///@brief Amount by which the camera should move by along x-axis
	float dx,
		///@brief Amount by which the camera should move by along y-axis
		dy,
		///@brief Amount by which the camera should move by along z-axis
		dz;

	///@brief Amount by which the camera is zoomed in
	float zoom,
		///@brief Amount by which the camera zoom level is changed
		zoom_change;

	///@brief Amount by which the object is rotated along x-axis
	float rotx,
		///@brief Amount by which the object is rotated along y-axis
		roty,
		///@brief Amount by which the rotation of the object is changed
		rotch;
	bool return_to_console;

  public:
	/**
	 * @brief Construct a new window object
	 * 
	 * Initializes GLFW,GLEW,ImGui
	 * 
	 * @param _width width of the window (default = 800)
	 * @param _height height of the window (default = 800)
	 */
	window(int _width = 800, int _height = 800);

	/**
	 * @brief Destroy the window object
	 * 
	 * Terminates GLFW and ImGui
	 * 
	 */
	~window();

	/**
	 * @brief Process keyboard events with 
	 * help of ImGuiIO
	 * 
	 */
	void event_keyboard();

	/**
	 * @brief A callback to change width and height of the window
	 */
	void window_refresh();

	/**
	 * @brief Callback for getting position of camera
	 * 
	 * @return pos 
	 */
	inline pos get_pos()
	{
		return p;
	}

	/**
	 * @brief Callback for getting camera's zoom level
	 * 
	 * @return float 
	 */
	inline float get_zoom()
	{
		return zoom;
	}

	/**
	 * @brief Get the width of the window
	 * 
	 * @return int 
	 */
	inline int get_width()
	{
		return width;
	}

	/**
	 * @brief Get the height of the window
	 * 
	 * @return int 
	 */
	inline int get_height()
	{
		return height;
	}

	/**
	 * @brief Get the x rotation of the model
	 * 
	 * @return float 
	 */
	inline float get_rotx()
	{
		return rotx;
	}

	/**
	 * @brief Get the y rotation of the model
	 * 
	 * @return float 
	 */
	inline float get_roty()
	{
		return roty;
	}

	/**
	 * @brief Abstraction for swapping window buffers
	 * 
	 */
	inline void swap_buffers()
	{
		glfwSwapBuffers(_window);
	}

	/**
	 * @brief Abstraction for handling events
	 * 
	 */
	inline void poll_events()
	{
		glfwPollEvents();
		event_keyboard();
	}

	/**
	 * @brief For checking whether the window should be closed
	 * 
	 * If the user clicks on the X button the window should be closed
	 * 
	 * @return true if window should be close
	 * @return false else
	 */
	inline bool should_close()
	{
		return glfwWindowShouldClose(_window);
	}
};

#endif