#ifndef SOLVER2D_H
#define SOLVER2D_H

/**
 * @file solver2d.h
 * @brief This file defines necessary functions
 * required to solve a differential equation on a 2d mesh
 * @date 2019-04-20
 * 
 */
#include "2d.h"
#include "mesh2d.h"
#include "window.h"
#include "gl_abstraction.h"

namespace __2d__
{

namespace solver
{

struct triangle : public __2d__::triangle
{
	pos centroid;
	double value = 0;
	double length_a = 0,length_b = 0,length_c = 0;
	double angle_a = 0,angle_b = 0,angle_c = 0;
	std::vector<triangle_id> S;
	triangle(const __2d__::triangle t)
	{
		a = t.a;
		b = t.b;
		c = t.c;
		id = t.id;
		type = t.type;
		E = t.E;
		value = 0.0;

	}
};

class solver
{
	private:
	///@brief The collection of unique points that the mesh contains
	std::vector<node> N;
	///@brief The collection of unique edges that the mesh contains
	std::vector<edge> E;
	///@brief The collection of unique triangles that the mesh contains
	std::vector<triangle> T;

	/**
	 * @brief A boolean to specify whether ghost triangles are generated
	 * 
	 * Once this boolean is set to true no further changes can be made to the mesh
	 * 
	 */
	bool ghost_generated = false;

	/**
	 * @brief Generates ghost triangles
	 */
	void generate_ghosts();

	/**
	 * @brief Makes each triangle aware of its neighbouring triangles
	 */
	void link_triangles();

	/**
	 * @brief Calculates the properties of all the triangles
	 */
	void fill_in_data();

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return vector of all node positions as 3 floats
	 */
	std::vector<float> export_vertex_data() const;

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return index of all nodes as unsigned integer
	 */
	std::vector<uint32_t> export_node_index() const;

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return index of all nodes of the edges as unsigned integer
	 */
	std::vector<uint32_t> export_edge_index() const;

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return index of all nodes of the edges that belong to a ghost triangle
	 * as unsigned integer
	 */
	std::vector<uint32_t> export_ghost_edge_index() const;

	public:
	/**
	 * @brief Imports a 2d mesh from the mesh class
	 * @param M 
	 */
	void import_mesh(const mesh &M);

	/**
	 * @brief Displays the result to the screen
	 */
	void display();

	void apply_boundary_condition_default();
	/**
	 * @brief 
	 * 
	 */
	void solve();

	std::tuple<std::vector<uint32_t>,std::vector<color>> visualize_solution();

	void update_node_values();

};

}
}

#endif