#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "2d.h"

#define epsilon 1.0e-14
#define area_threshold 1.0e-3

enum class mesh_type
{
	_2d,
	_3d
};

class mesh
{
  private:
	node *N;
	edge *E;
	mesh_triangle *T;
	tetrahedron *TH;
	int number_of_nodes = 0, number_of_edges = 0, number_of_triangles = 0, number_of_tetrahedrons;
	int max_nodes, max_edges, max_triangles, max_tetrahedrons;
	GLFWwindow *window = nullptr;
	mesh_type type;
	bool imported = false;
	bool ghost_generated = false;
	bool basic_generation = false;

	/*Creates a new triangle appends it the array and
		links it with its nodes	*/
	void make_triangle(node *a, node *b, node *c, triangle_type = triangle_type::domain);

	/*Replaces a triangle on the array with a new triangle*/
	void replace_triangle(int id, node *a, node *b, node *c);

  public:
	void init_2d();

	//imports from 2d object
	friend void import_2d(mesh &, _2D_ &);

	//imports from face
	void import_face();

	//imports from 3d
	void import_3d();

	/*Generates a simple triangulation
		Details: Take a edge , take a node checks whether
		the triangle formed is valid i.e triangle lies within the domain,
		and triangle has a non zero area*/
	void generate_mesh_basic();

	/*Inserts a node into the domain
		at the centroid of the polygon formed by
		the triangles that share a common boundary node*/
	void node_insertion();

	/*Forms 3 triangles by joining each of the triangle's edge with the 
		centroid of the triangle*/
	void refine_triangles();
	void refine_triangles_near_boundary(node_location);

	/*Swaps the edge of 2 adjacent triangles
		if the minimum angle increasese after swapping*/
	void edge_swap();

	/*Considers an inside node and changes it's
		position to the centroid of the polygon formed
		by all the triangles that share a common node inside the
		domain*/
	void centroid_shift();

	/*Generates a complete mesh
		Details: First generates basic mesh next does node insertion,
		equalize triangles and edge swap, finishes with centroid shift*/
	void generate_mesh_full();

	//Generates ghost triangles
	void generate_ghosts();
	void generate_ghosts_new();

	inline void attach_window(GLFWwindow *_window)
	{
		window = _window;
	}

	//Displays the mesh onto the screen
	void display();
	void imp_display();

	//Helps to inspect the mesh closely for any defects
	void inspect();

	void input();

	//Displays the number of nodes, triangle
	//and the average area of the triangles in the mesh
	void stats();

	//Returns the average area of all the triangles in the mesh
	double avg_area_of_triangles();

	//Returns the average area of the triangles which are near a boundary or a hole in the mesh
	double avg_area_of_triangles_near_boundary(node_location);

	//Sweeps the entire mesh linking the triangles with it's nodes
	void node_triangle_share_sweep();
};

//updated on 23/8/18
//This function is only used by generate_mesh_basic
inline bool left_test_2d(edge e, node n)
{
	return left_test_2d({e.start->p, e.end->p}, n.p);
}

//updated on 23/8/18
//This function is only used by generate_mesh_basic
inline bool collinear_test(edge e, node n)
{
	return collinear_test({e.start->p, e.end->p}, n.p);
}

//updated on 23/8/18
//This function is only used by generate_mesh_basic
inline bool intersection_test(edge e, edge *E, int n)
{
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		if (do_they_intersect({E[i].start->p, E[i].end->p}, {e.start->p, e.end->p}))
		{
			++count;
			break;
		}
	}
	if (count == 0)
		return true;
	else
		return false;
}

//updated on 23/8/18
//Returns the number of edges that have not been used by the basic mesh generator
//This function is only used by generate_mesh_basic
inline int number_of_unused_edges(edge *E, int n)
{
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		if (E[i].availability == true)
			count++;
	}
	return count;
}

edge *edge_exists(edge *E, int n, edge e);

void disable_common_node(edge *a, edge *b);

void generate_unique_pos(node &n, pos *p);

std::pair<node *, node *> corner_pos(const node &n);

pos generate_centroid_for_polygon(pos *p, int n);


pos generate_ghost_point(triangle t, pos p);

bool connected_node(node *a, node *b);

edge find_common_edge(mesh_triangle *t1, mesh_triangle *t2);

node *vertex_opposite_to_triangle_edge(mesh_triangle *t, edge e);

int find_triangle_containing_edge(edge &e);
