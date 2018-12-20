#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "position.h"
#include "2d.h"
#include "display.h"

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

//updated on 23/8/18
//Before forming a new edge to create a triangle checks whether an edge is already present in the location
//This function is only used by generate_mesh_basic
inline edge *edge_exists(edge *E, int n, edge e)
{
	for (int i = 0; i < n; i++)
	{
		if ((E[i].start->p == e.start->p && E[i].end->p == e.end->p) || (E[i].start->p == e.end->p && E[i].end->p == e.start->p))
		{
			return &E[i];
		}
	}
	return 0;
}

//updated on 23/8/18
//When a triangle is formed between two adjacent edges their common node is diabled
//This function is only used by generate_mesh_basic
inline void disable_common_node(edge *a, edge *b)
{
	if (a->start->p == b->start->p)
		b->start->availability = false;
	else if (a->end->p == b->start->p)
		b->start->availability = false;
	else if (a->start->p == b->end->p)
		b->end->availability = false;
	else if (a->end->p == b->end->p)
		b->end->availability = false;
}

//updated on 23/8/18
//Generates an array of unique nodes from a node's shared triangles
//This function is only used by node_insertion
inline void generate_unique_pos(node &n, pos *p)
{
	int count = 0;
	pos temp;
	for (int i = 0; i < n.share; i++)
	{
		if (count == 0)
		{
			p[count] = n.T[i]->a->p;
			count++;
			p[count] = n.T[i]->b->p;
			count++;
			p[count] = n.T[i]->c->p;
			count++;
		}

		temp = n.T[i]->a->p;
		if (unique_pos(temp, p, count))
		{
			p[count] = temp;
			count++;
		}

		temp = n.T[i]->b->p;
		if (unique_pos(temp, p, count))
		{
			p[count] = temp;
			count++;
		}

		temp = n.T[i]->c->p;
		if (unique_pos(temp, p, count))
		{
			p[count] = temp;
			count++;
		}
	}
}

//updated on 23/8/18
//Returns a pair of nodes that are on either side of the node in consideration on the boundary or on thehole
//This function is only used by node_insertion
inline std::pair<node *, node *> corner_pos(const node &n)
{
	if (n.p == n.BE[0]->start->p)
	{
		return std::make_pair(n.BE[0]->end, n.BE[1]->start);
	}

	else if (n.p == n.BE[0]->end->p)
	{
		return std::make_pair(n.BE[0]->start, n.BE[1]->end);
	}

	//avoiding Wreturn-type
	return std::make_pair(n.BE[0]->start,n.BE[1]->end);
}

//updated on 23/8/18
//Finds the centroid of the polygon formed by the triangles sharing a common node
//This function is only used by node_insertion
inline pos generate_centroid_for_polygon(pos *p, int n)
{
	pos result{};

	for (int i = 0; i < n; i++)
	{
		result = result + p[i];
	}
	result = result / n;
	return result;
}

//updated on 23/8/18
/*Generates a ghost point by reflecting the third vertex(not on boundary or on hole) of the triangle on the boundary
about it's opposite side*/
//This function is only used by generate_ghosts
inline pos generate_ghost_point(triangle t, pos p)
{
	pos res;
	double angle;
	if (t.a == p)
	{
		angle = line_inclination_absolute({t.b, t.c});
		p = p - t.b;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res = res + t.b;
		return res;
	}

	else if (t.b == p)
	{
		angle = line_inclination_absolute({t.c, t.a});
		p = p - t.c;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res = res + t.c;
		return res;
	}

	else if (t.c == p)
	{
		angle = line_inclination_absolute({t.a, t.b});
		p = p - t.a;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res = res + t.a;
		return res;
	}

	return res;
}

//updated on 23/8/18
//Checks whether a side of the triangle consists of only on edge element in the boundary
//This function is only used by generate_ghosts
inline bool connected_node(node *a, node *b)
{
	if (a == a->BE[0]->start && b == a->BE[0]->end)
	{
		return true;
	}

	else if (a == a->BE[0]->end && b == a->BE[0]->start)
	{
		return true;
	}

	if (a == a->BE[1]->start && b == a->BE[1]->end)
	{
		return true;
	}

	else if (a == a->BE[1]->end && b == a->BE[1]->start)
	{
		return true;
	}

	else
	{
		return false;
	}
}

//updated on 23/8/18
//Finds the common edge between two triangles if it exists else returns a edge with zero length
//This function is only used by edge_swap
inline edge find_common_edge(mesh_triangle *t1, mesh_triangle *t2)
{

	edge e;
	if (((t1->a->p == t2->a->p) || (t1->a->p == t2->b->p) || (t1->a->p == t2->c->p)) && ((t1->b->p == t2->a->p) || (t1->b->p == t2->b->p) || (t1->b->p == t2->c->p)))
	{
		e.start = t1->a;
		e.end = t1->b;
		return e;
	}
	else if (((t1->b->p == t2->a->p) || (t1->b->p == t2->b->p) || (t1->b->p == t2->c->p)) && ((t1->c->p == t2->a->p) || (t1->c->p == t2->b->p) || (t1->c->p == t2->c->p)))
	{
		e.start = t1->b;
		e.end = t1->c;
		return e;
	}
	else if (((t1->c->p == t2->a->p) || (t1->c->p == t2->b->p) || (t1->c->p == t2->c->p)) && ((t1->a->p == t2->a->p) || (t1->a->p == t2->b->p) || (t1->a->p == t2->c->p)))
	{
		e.start = t1->c;
		e.end = t1->a;
		return e;
	}
	else
	{
		node a, b;
		a = {};
		b = {};
		e.start = &a;
		e.end = &b;
		return e;
	}
}

//updated on 23/8/18
//Finds the vertex opposite to the common edge
//This function is only used by edge_swap
inline node *vertex_opposite_to_triangle_edge(mesh_triangle *t, edge e)
{
	if (t->a->p != e.start->p && t->a->p != e.end->p)
		return t->a;
	else if (t->b->p != e.start->p && t->b->p != e.end->p)
		return t->b;
	else if (t->c->p != e.start->p && t->c->p != e.end->p)
		return t->c;
	
	return nullptr;
}

inline int find_triangle_containing_edge(edge &e)
{
	int id = -1;
	for (int i = 0; i < e.start->share; i++)
		for (int j = 0; j < e.end->share; j++)
		{
			if (e.start->T[i]->id == e.end->T[j]->id)
			{
				id = e.end->T[j]->id;
			}
		}
	return id;
}