#ifndef MESH2D_H
#define MESH2D_H
#include "2d.h"
#include "window.h"
#include "model2d.h"

namespace __2d__
{

class mesh
{
  private:
	std::vector<node> N;
	std::vector<edge> E;
	std::vector<triangle> T;
	bool imported = false;
	bool ghost_generated = false;
	bool basic_generation = false;

	//Common
	inline const uint64_t number_of_nodes()
	{
		return N.size();
	}

	pos generate_centroid(const std::vector<node> &);

	//////////////////////////////////////////////
	//2D funstions
	/*Creates a new triangle appends it to the vector and
		links it with its nodes	*/
	void make_triangle(const node_id, const node_id, const node_id, triangle_type = triangle_type::domain);
	void make_inside_edge(const node_id, const node_id, bool);

	inline const uint64_t number_of_edges()
	{
		return E.size();
	}

	inline const uint64_t number_of_triangles()
	{
		return T.size();
	}

	//This function is only used by generate_mesh_basic
	inline bool left_test(const edge &e, const node &n)
	{
		return left_test_2d({N[e.start].p, N[e.end].p}, n.p);
	}

	//This function is only used by generate_mesh_basic
	inline bool collinear_test(const edge &e, const node &n)
	{
		return !is_collinear(line(N[e.start].p, N[e.end].p), n.p);
	}

	//This function is only used by generate_mesh_basic
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

	//Returns the number of edges that have not been used by the basic mesh generator
	//This function is only used by generate_mesh_basic
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

	inline const double edge_length(const edge &e)
	{
		return distance(N[e.start].p, N[e.end].p);
	}

	inline const double triangle_area(const triangle &m)
	{
		return area_of_triangle(N[m.a].p, N[m.b].p, N[m.c].p);
	}

	int64_t edge_exists(const edge &);
	void disable_common_node(const edge &, const edge &);
	std::pair<node_id, node_id> corner_pos(const node &);

	inline double triangle_min_angle(const triangle_id t_id)
	{
		return min_angle_of_triangle(N[T[t_id].a].p, N[T[t_id].b].p, N[T[t_id].c].p);
	}
	const node_id vertex_opposite_to_triangle_edge(const triangle &, const edge &);

	pos generate_ghost_point(const triangle_id, const node_id);

	void triangle_node_change(const triangle_id, const node_id, const node_id);
	void edge_node_change(const edge_id, const node_id, const node_id);

	//Sweeps the entire mesh linking the triangles with it's nodes
	void node_triangle_share_sweep();
	void node_edge_share_sweep();
	void edge_triangle_share_sweep();
	void triangle_edge_share_sweep();

	void display_node(const std::vector<node> &m_N);
	void display_triangle(const std::vector<triangle> &m_T);
	void display_edge(const std::vector<edge> &m_E);
	void display_all_nodes();
	void display_all_edges();
	void display_all_triangles();
	//void update_pdata();
	void display_internal(window &w);

	std::vector<float> export_vertex_data() const;
	std::vector<uint32_t> export_node_index() const;
	std::vector<uint32_t> export_edge_index() const;
  public:
	//imports from 2d object
	void import_2d(const model&);

	//Displays the number of nodes, triangle
	//and the average area of the triangles in the mesh
	void stats();

	//Returns the average area of all the triangles in the mesh
	double avg_area_of_triangles();

	//Returns the average area of the triangles which are near a boundary or a hole in the mesh
	double avg_area_of_triangles_near_boundary(node_location);

	#ifdef MESH_DEBUG

	void generate_mesh_basic_debug();

	/*Inserts a node into the domain
		at the centroid of the polygon formed by
		the triangles that share a common boundary node*/
	void node_insertion_debug();

	/*Forms 3 triangles by joining each of the triangle's edge with the 
		centroid of the triangle*/
	void refine_triangles_debug();
	void refine_triangles_near_boundary_debug(node_location);

	/*Swaps the edge of 2 adjacent triangles
		if the minimum angle increasese after swapping*/
	void edge_swap_debug();

	/*Considers an inside node and changes it's
		position to the centroid of the polygon formed
		by all the triangles that share a common node inside the
		domain*/
	void centroid_shift_debug();

	#endif

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
	void inline refine_triangles_near_bound()
	{
		refine_triangles_near_boundary(__2d__::node_location::boundary);
	}
	void inline refine_triangles_near_hole()
	{
		refine_triangles_near_boundary(__2d__::node_location::hole);
	}

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

	
	//Displays the mesh onto the screen
	void display();
	void generate_interactive();
	

	void save(const std::string& filepath);
	void load(const std::string& filepath);
};

} // namespace __mesh2d__

#endif