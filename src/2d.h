#pragma once
#include "mesh_basic.h"
#include "display.h"

class _2D_
{
  private:
	std::vector<node> N;
	std::vector<edge> E;
	GLFWwindow *window = nullptr;
	void intersections(std::vector<line> &, std::vector<std::pair<line, line>> &);
	void branches_and_gaps(std::vector<line> &, std::vector<pos> &);
	bool intersection_pass, branch_pass;

	inline const uint64_t number_of_nodes()
	{
		return N.size();
	}

	inline const uint64_t number_of_edges()
	{
		return E.size();
	}

  public:
	bool transferable = true;

	//Reads a dxf file and converts it to a more usable form
	void dxf_read(const std::string filepath, double dx = 1.0);

	//Reads from the cache created from a previously used dxf file
	void cache_read();

	//Writes a temporary cache from reading a dxf file
	void cache_write();

	/*Simple boundary constructs for testing purposes*/
	void add_boundary_circle(double radius, double dx);
	void add_boundary_square(double length, double dx);
	void add_hole_circle(pos centre, double radius, double dx);
	void add_hole_square(pos centre, double length, double dx);

	//Attaches a window created by GLFW for displaying
	inline void attach_window(GLFWwindow *_window)
	{
		window = _window;
	}

	//Displays the 2d figure
	void display();

	//Displays Errors if any when reading from a dxf file
	void display_import_abnormalities();

	//Transfers the data from the 2D class to the mesh class
	friend void import_2d(mesh &,const _2D_ &);
};