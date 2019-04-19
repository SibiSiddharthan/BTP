#ifndef MESH3D_H
#define MESH3D_H

/**
 * @file mesh3d.h
 * @brief This file defines functions necessary to generate
 * a 3d mesh
 * @date 2019-04-19
 * 
 */
#include "window.h"
#include "model3d.h"
#include "gl_abstraction.h"

namespace __3d__
{

class mesh
{
  private:
	std::vector<node> N;
	std::vector<plane> P;
	std::vector<tetrahedron> TH;
	bool imported = false;
	bool ghost_generated = false;
	bool basic_generation = false;

	//Common
	inline const uint64_t number_of_nodes()
	{
		return N.size();
	}

	pos generate_centroid(const std::vector<node> &);

	//3D
	void make_inside_plane(const node_id, const node_id, const node_id, bool);
	void make_tetrahedron(const node_id, const node_id, const node_id, const node_id, tetrahedron_type = tetrahedron_type::domain);

	inline const uint64_t number_of_planes()
	{
		return P.size();
	}

	inline const uint64_t number_of_tetrahedrons()
	{
		return TH.size();
	}

	inline bool left_test(const plane &p, const node &n)
	{
		return left_test_3d({N[p.a].p, N[p.b].p, N[p.c].p, p.normal}, n.p);
	}

	

	inline bool coplanar_test(const plane &p, const node &n)
	{
		return !is_coplanar({N[p.a].p, N[p.b].p, N[p.c].p, p.normal}, n.p);
	}

	#ifdef MESH_DEBUG
	inline double left_test_debug(const plane &p, const node &n)
	{
		pos centroid = (N[p.a].p + N[p.b].p + N[p.c].p) / 3;
		pos vec = n.p - centroid;
		pos normal = centroid + p.normal * 0.5;
		return vec.x * normal.x + vec.y * normal.y + vec.z * normal.z;
	}

	inline double coplanar_test_debug(const plane &p, const node &n)
	{
		return fabs((dot(cross(N[p.a].p - n.p, N[p.b].p - n.p), N[p.c].p - n.p))) / 6;
	}

	inline std::vector<size_t> intersection_test_debug(const plane &p, const std::vector<plane> &P)
	{
		uint64_t count = 0;
		std::unordered_set<node_id> n_p = {p.a, p.b, p.c};
		std::vector<size_t> ll;

		for (const plane &m_p : P)
		{
			if (n_p.find(m_p.a) == n_p.end() && n_p.find(m_p.b) == n_p.end() && n_p.find(m_p.c) == n_p.end())
			{
				if (do_they_intersect({N[m_p.a].p, N[m_p.b].p, N[m_p.c].p, m_p.normal}, {N[p.a].p, N[p.b].p, N[p.c].p, p.normal}))
				{
					++count;
					break;
				}
			}
		}

		return ll;
	}
	#endif

	inline bool intersection_test(const plane &p, const std::vector<plane> &P)
	{
		uint64_t count = 0;
		std::unordered_set<node_id> n_p = {p.a, p.b, p.c};

		for (const plane &m_p : P)
		{
			if (n_p.find(m_p.a) == n_p.end() && n_p.find(m_p.b) == n_p.end() && n_p.find(m_p.c) == n_p.end())
			{
				if (do_they_intersect({N[m_p.a].p, N[m_p.b].p, N[m_p.c].p, m_p.normal}, {N[p.a].p, N[p.b].p, N[p.c].p, p.normal}))
				{
					++count;
					break;
				}
			}
		}
		if (count == 0)
			return true;
		else
			return false;
	}

	inline uint64_t number_of_unused_planes(const std::vector<plane> &P)
	{
		uint64_t count = 0;
		for (const plane &p : P)
		{
			if (p.availability == true)
				count++;
		}
		return count;
	}

	inline const double tetrahedron_volume(const tetrahedron &te)
	{
		return volume_of_tetrahedron(N[te.a].p, N[te.b].p, N[te.c].p, N[te.d].p);
	}

	int64_t plane_exists(const plane &);
	void disable_common_node(const plane &, const plane &);

	
	std::vector<float> pdata;
	void update_pdata();

	void node_tetrahedron_share_sweep();
	void node_plane_share_sweep();
	void plane_tetrahedron_share_sweep();
	void tetrahedron_plane_share_sweep();

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
	 * @return index of all nodes of the planes as unsigned integer
	 */
	std::vector<uint32_t> export_plane_index() const;
	
	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return index of all nodes of the edges of the planes as unsigned integer
	 */
	std::vector<uint32_t> export_plane_edge_index() const;

	/**
	 * @brief This is a helper function used for displaying the model.
	 * Exports normal coordinates and index data
	 * 
	 * The normals are drawn from the centre of each plane to diatance
	 * of 0.1 units in the direction of the normal
	 */
	std::tuple<std::vector<float>,std::vector<uint32_t>> export_normals() const;

	

	void display_internal(window &w, bool draw_normals);
	void display_nodes(const std::vector<node> &m_N);
	void display_planes(const std::vector<plane> &m_P,bool draw_normals,color m_color = colors("turquoise"));
	void display_tetrahedrons(const std::vector<tetrahedron> &m_T);

  public:
	//imports from 3d
	void import_3d(const model &);

	#ifdef MESH_DEBUG
	void generate_mesh_basic_debug();
	#endif
	//double avg_volume_of_tetrahedrons();
	//double avg_volume_of_tetrahedrons_near_boundary(node_location);

	//void refine_tetrahedrons();
	//void refine_tetrahedrons_near_boundary(node_location);
	//void centroid_shift_3d();
	//void node_insertion_3d();
	//void plane_swap();
	void generate_mesh_basic();
	void display();
	//void node_share_dsiplay();
};

} // namespace __mesh3d__

#endif