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

namespace __2d__
{

namespace solver
{

struct triangle : __2d__::triangle
{

	triangle(const __2d__::triangle t)
	{

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

	void link_triangles();

	public:
	/**
	 * @brief Imports a 2d mesh from the mesh class
	 * 
	 * @param M 
	 */
	void import_mesh(const mesh &M);

	/**
	 * @brief Generates ghost triangles
	 * 
	 */
	void generate_ghosts();


	void solve();

};

}
}

#endif