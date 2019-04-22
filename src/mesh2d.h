#ifndef MESH2D_H
#define MESH2D_H

/**
 * @file mesh2d.h
 * @brief This file declares the 2d mesh class
 * which contains the machinery to generate a 2d mesh
 * @date 2019-04-18
 * 
 */
#include "2d.h"
#include "window.h"
#include "model2d.h"



namespace __2d__
{

/**
 * @brief The mesh class which defines the functions
 * neccessary to generate a 2d mesh
 */
class mesh
{
  private:
	///@brief The collection of unique points that the mesh contains
	std::vector<node> N;
	///@brief The collection of unique edges that the mesh contains
	std::vector<edge> E;
	///@brief The collection of unique triangles that the mesh contains
	std::vector<triangle> T;

	bool imported = false;
	/**
	 * @brief A boolean to specify whether ghost triangles are generated
	 * 
	 * Once this boolean is set to true no further changes can be made to the mesh
	 * 
	 */
	bool ghost_generated = false;

	/**
	 * @brief A boolean to specify whether the first stage of mesh generation 
	 * is performed or not
	 * 
	 * Once the boolean is set the basic generation function cannot be called again
	 * 
	 */
	bool basic_generation = false;

	/**
	 * @brief Returns the number of nodes
	 */
	inline const uint64_t number_of_nodes()
	{
		return N.size();
	}

	/**
	 * @brief Returns the number of edges
	 */
	inline const uint64_t number_of_edges()
	{
		return E.size();
	}

	/**
	 * @brief Returns the number of triangles
	 */
	inline const uint64_t number_of_triangles()
	{
		return T.size();
	}

	/**
	 * @brief Given a polygon of points this function
	 * calculates the centroid of the polygon
	 * 
	 * @return centroid of the polygon 
	 */
	pos generate_centroid(const std::vector<node> &);

	/**
	 * @brief Creates a new triangle appends it to the vector and makes  
	 * it's nodes and edges aware of it's presence
	 * 
	 */
	void make_triangle(const node_id, const node_id, const node_id, triangle_type = triangle_type::domain);

	/**
	 * @brief Create an edge inside the domain
	 */
	void make_inside_edge(const node_id, const node_id, bool);

	/**
	 * @brief Checks if the node n lies to the right side of edge e
	 * 
	 * This function is only used by generate_mesh_basic
	 * 
	 * @param e 
	 * @param n 
	 * @return true 
	 * @return false 
	 */
	inline bool left_test(const edge &e, const node &n)
	{
		return left_test_2d({N[e.start].p, N[e.end].p}, n.p);
	}

	/**
	 * @brief Checks if the node n and edge e are collinear
	 * 
	 * This function is only used by generate_mesh_basic
	 * 
	 * @param e 
	 * @param n 
	 * @return true 
	 * @return false 
	 */
	inline bool collinear_test(const edge &e, const node &n)
	{
		return !is_collinear(line(N[e.start].p, N[e.end].p), n.p);
	}

	/**
	 * @brief Checks whether edge e intersects with any other edges of present in the mesh
	 * 
	 * This function is only used by generate_mesh_basic
	 * @param e 
	 * @param E 
	 * @return true 
	 * @return false 
	 */
	inline bool intersection_test(const edge &e, const std::vector<edge> &E)
	{
		uint64_t count = 0;
		for (const edge &m_e : E)
		{
			if (do_they_intersect(line({N[m_e.start].p, N[m_e.end].p}), line({N[e.start].p, N[e.end].p})))
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

	/**
	 * @brief Returns the number of edges that have not been used by the basic mesh generator
	 * 
	 * This function is only used by generate_mesh_basic
	 * @param E 
	 */
	inline uint64_t number_of_unused_edges(const std::vector<edge> &E)
	{
		uint64_t count = 0;
		for (const edge &e : E)
		{
			if (e.availability == true)
				count++;
		}
		return count;
	}

	/**
	 * @brief Returns the lenght of edge e
	 * 
	 * @param e 
	 */
	inline const double edge_length(const edge &e)
	{
		return distance(N[e.start].p, N[e.end].p);
	}

	/**
	 * @brief Returns the area of triangle t
	 * 
	 * @param t 
	 */
	inline const double triangle_area(const triangle &t)
	{
		return area_of_triangle(N[t.a].p, N[t.b].p, N[t.c].p);
	}

	/**
	 * @brief Returns the index of the edge is it exists or
	 * else returns -1
	 * 
	 * This function is only used by generate_mesh_basic
	 */
	int64_t edge_exists(const edge &);

	/**
	 * @brief Changes node::availabilty to false
	 * if that node is common to edge a and b
	 * 
	 * This function is only used by generate_mesh_basic
	 * 
	 * @param a
	 * @param b
	 */
	void disable_common_node(const edge &a, const edge &b);

	/**
	 * @brief Finds the index of the nodes of the edges
	 * present in the boundary that have node n as a common node
	 * 
	 * @param n 
	 * @return pair of node ids 
	 */
	std::pair<node_id, node_id> corner_pos(const node &n);

	/**
	 * @brief Finds minimum angle of the triangle with id t_id
	 * 
	 * This function is used by edge swap
	 * 
	 * @param t_id 
	 * @return minimum angle in radians 
	 */
	inline double triangle_min_angle(const triangle_id t_id)
	{
		return min_angle_of_triangle(N[T[t_id].a].p, N[T[t_id].b].p, N[T[t_id].c].p);
	}
	
	/**
	 * @brief Returns the node id which is opposite to edge e contained
	 * in triangle t
	 * 
	 * @param t 
	 * @param e 
	 */
	const node_id vertex_opposite_to_triangle_edge(const triangle &t, const edge &e);

	/**
	 * @brief Calculates the position of the ghost point generated
	 * for triangle with id t_id
	 * 
	 * @param t_id 
	 * @param n_id 
	 * @return pos 
	 */
	pos generate_ghost_point(const triangle_id t_id, const node_id n_id);

	/**
	 * @brief Changes the node of triangle from fn_id to tn_id
	 * 
	 * @param t_id The id of the triangle
	 * @param fn_id The id of the node of the triangle that is to be changed
	 * @param tn_id The id of node that the triangle is changed to
	 */
	void triangle_node_change(const triangle_id t_id, const node_id fn_id, const node_id tn_id);

	/**
	 * @brief Changes the node of edge from fn_id to tn_id
	 * 
	 * @param e_id The id of the edge
	 * @param fn_id The id of the node of the edge that is to be changed
	 * @param tn_id The id of node that the edge is changed to
	 */
	void edge_node_change(const edge_id e_id, const node_id fn_id, const node_id tn_id);

	//Sweeps the entire mesh linking the triangles with it's nodes
	/**
	 * @brief Makes every node aware of the triangles it is part of
	 */
	void node_triangle_share_sweep();

	/**
	 * @brief Makes every node aware of the edges it is part of
	 */
	void node_edge_share_sweep();

	/**
	 * @brief Makes every edge aware of the triangles it is part of
	 */
	void edge_triangle_share_sweep();

	/**
	 * @brief Makes every triangle aware of the edges that it contains
	 */
	void triangle_edge_share_sweep();

	/**
	 * @brief This function is used for debug purposes
	 * Displays a particular list of nodes
	 * 
	 * @param m_N The list of nodes
	 */
	void display_node(const std::vector<node> &m_N);

	/**
	 * @brief This function is used for debug purposes
	 * Displays a particular list of triangles
	 * 
	 * @param m_T The list of triangles
	 */
	void display_triangle(const std::vector<triangle> &m_T);

	/**
	 * @brief This function is used for debug purposes
	 * Displays a particular list of edges
	 * 
	 * @param m_E The list of edges
	 */ 
	void display_edge(const std::vector<edge> &m_E);
	void display_all_nodes();
	void display_all_edges();
	void display_all_triangles();
	//void update_pdata();

	/**
	 * @brief This function is used for debug purposes
	 * Displays the entire mesh to the window w
	 * 
	 * @param w 
	 */
	void display_internal(window &w);

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
  public:
	/**
	 * @brief imports from 2d model object
	 * 
	 * @param M The 2d model
	 */
	void import_2d(const model &M);

	/**
	 * @brief Displays the number of nodes, triangle
	 * and the average area of the triangles in the mesh
	 * 
	 */
	void stats();
	
	/**
	 * @brief Returns the average area of all the triangles in the mesh
	 */
	double avg_area_of_triangles();
	
	/**
	 * @brief Returns the average area of the triangles which are near a boundary or a hole in the mesh
	 */
	double avg_area_of_triangles_near_boundary(node_location);

	/**
	 * @def MESH_DEBUG
	 * @brief The define introduces functions
	 * that help debug a mesh generation algorithm
	 */
	#ifdef MESH_DEBUG

	/**
	 * @brief Debug version of generate_mesh_basic
	 */
	void generate_mesh_basic_debug();

	/*Inserts a node into the domain
		at the centroid of the polygon formed by
		the triangles that share a common boundary node*/
	/**
	 * @brief Debug version of node_insertion
	 */
	void node_insertion_debug();

	/*Forms 3 triangles by joining each of the triangle's edge with the 
		centroid of the triangle*/
	/**
	 * @brief Debug version of refine triangles
	 */
	void refine_triangles_debug();

	/**
	 * @brief Debug version of refine triangles near boundary
	 */
	void refine_triangles_near_boundary_debug(node_location);

	/*Swaps the edge of 2 adjacent triangles
		if the minimum angle increasese after swapping*/
	/**
	 * @brief Debug version of edge swap
	 */
	void edge_swap_debug();

	/*Considers an inside node and changes it's
		position to the centroid of the polygon formed
		by all the triangles that share a common node inside the
		domain*/
	/**
	 * @brief Debug version centroid_shift
	 */
	void centroid_shift_debug();

	#endif

	/*Generates a simple triangulation
		Details: Take a edge , take a node checks whether
		the triangle formed is valid i.e triangle lies within the domain,
		and triangle has a non zero area*/
	/**
	 * @brief The first step of my mesh generation algorithm
	 * 
	 * Generates a simple triangulation\n
	 * Take a edge , take a node checks whether
	 * the triangle formed is valid i.e triangle lies within the domain(left test),
	 *  triangle has a non zero area(collinear test) and edges of the triangle
	 * don't intersect with any other edges(intersection test)
	 * 
	 */
	void generate_mesh_basic();

	/**
	 * @brief Inserts a node into the domain
	 * at the centroid of the polygon formed by
	 * the triangles that share a common boundary node
	 * 
	 */
	void node_insertion();

	/**
	 * @brief Forms 3 triangles by joining each of the triangle's edge with the 
	 * centroid of the triangle
	 * 
	 */
	void refine_triangles();

	/**
	 * @brief Forms 3 triangles by joining each of the triangle's edge with the 
	 * centroid of the triangle. This function only affects triangles near the boundary
	 */
	void refine_triangles_near_boundary(node_location);

	/**
	 * @brief Swaps the edge of 2 adjacent triangles
	 * if the minimum angle of both the triangles increases after swapping
	 */
	void edge_swap();

	/**
	 * @brief Considers an inside node and changes it's
	 * position to the centroid of the polygon formed
	 * by all the triangles that share a common node inside the
	 * domain
	 */
	void centroid_shift();

	/**
	 * @brief Generates a complete mesh
	 * 
	 * First generates basic mesh next does node insertion,
	 * equalize triangles and edge swap, finishes with centroid shift
	 */
	void generate_mesh_full();

	/**
	 * @brief Generates ghost triangles
	 * 
	 */
	void generate_ghosts();

	
	/**
	 * @brief Displays the mesh onto the screen
	 */
	void display();

	/**
	 * @brief An interactive way of generating the mesh
	 */
	void generate_interactive();
	
	/**
	 * @brief Saves the generated mesh to a file
	 * 
	 * @param filepath 
	 */
	void save(const std::string& filepath);

	/**
	 * @brief Loads the mesh from a file
	 * 
	 * @param filepath 
	 */
	void load(const std::string& filepath);

	/**
	 * @brief Exports the mesh to the solver class
	 * 
	 * @return std::tuple<std::vector<node>,std::vector<edge>,std::vector<triangle>> 
	 */
	std::tuple<std::vector<node>,std::vector<edge>,std::vector<triangle>> export_mesh() const;
};

} // namespace __mesh2d__

#endif