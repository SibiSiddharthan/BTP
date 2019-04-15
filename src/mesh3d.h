#ifndef MESH3D_H
#define MESH3D_H

#include "window.h"
#include "3d.h"
#include "model3d.h"

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

	//////////////////////////////////////////////

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

	inline bool collinear_test(const plane &p, const node &n)
	{
		return !is_coplanar({N[p.a].p, N[p.b].p, N[p.c].p, p.normal}, n.p);
	}

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

  public:
	//imports from 3d
	void import_3d(const model &);

	
	double avg_volume_of_tetrahedrons();
	double avg_volume_of_tetrahedrons_near_boundary(node_location);

	void refine_tetrahedrons();
	void refine_tetrahedrons_near_boundary(node_location);
	void centroid_shift_3d();
	void node_insertion_3d();
	void plane_swap();
	void generate_mesh_basic_3d();
	void display3d();
	void display3d_inspect();
	void node_share_dsiplay();
};

} // namespace __mesh3d__

#endif