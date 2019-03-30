#include "mesh3d.h"

using namespace std;

namespace __3d__
{

void mesh::import_3d(const model &M)
{
	auto [_N,_P] = M.model_export();
	N = _N;
	P = _P;
}

void mesh::refine_tetrahedrons()
{
	double avg = avg_volume_of_tetrahedrons();

	if (avg > volume_threshold)
	{
		vector<tetrahedron> temp;
		for (const tetrahedron &m_th : TH)
		{
			if (double volume = tetrahedron_volume(m_th); volume > 1.5 * avg && volume > 2 * volume_threshold)
			{

				N.push_back({(N[m_th.a].p + N[m_th.b].p + N[m_th.c].p + N[m_th.d].p) / 4.0, N.size(), node_location::inside, false});

				temp.push_back({N[N.size() - 1].id, m_th.b, m_th.c, m_th.d, temp.size()});
				temp.push_back({m_th.a, N[N.size() - 1].id, m_th.c, m_th.d, temp.size()});
				temp.push_back({m_th.a, m_th.b, N[N.size() - 1].id, m_th.d, temp.size()});
				temp.push_back({m_th.a, m_th.b, m_th.c, N[N.size() - 1].id, temp.size()});

				make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.b, false);
				make_inside_plane(N[N.size() - 1].id, m_th.b, m_th.c, false);
				make_inside_plane(N[N.size() - 1].id, m_th.c, m_th.d, false);
				make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.d, false);
			}
			else
				temp.push_back({m_th.a, m_th.b, m_th.c, m_th.d, temp.size()});
		}

		TH = temp;
	}
	node_tetrahedron_share_sweep();
}

void mesh::refine_tetrahedrons_near_boundary(node_location _location)
{
	double avg = avg_volume_of_tetrahedrons_near_boundary(_location);

	if (avg > volume_threshold)
	{
		vector<tetrahedron> temp;
		for (const tetrahedron &m_th : TH)
		{
			if (N[m_th.a].location == _location || N[m_th.b].location == _location || N[m_th.c].location == _location || N[m_th.d].location == _location)
			{
				if (double volume = tetrahedron_volume(m_th); volume > 1.5 * avg && volume > 2 * volume_threshold)
				{

					N.push_back({(N[m_th.a].p + N[m_th.b].p + N[m_th.c].p + N[m_th.d].p) / 4.0, N.size(), node_location::inside, false});

					temp.push_back({N[N.size() - 1].id, m_th.b, m_th.c, m_th.d, temp.size()});
					temp.push_back({m_th.a, N[N.size() - 1].id, m_th.c, m_th.d, temp.size()});
					temp.push_back({m_th.a, m_th.b, N[N.size() - 1].id, m_th.d, temp.size()});
					temp.push_back({m_th.a, m_th.b, m_th.c, N[N.size() - 1].id, temp.size()});

					make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.b, false);
					make_inside_plane(N[N.size() - 1].id, m_th.b, m_th.c, false);
					make_inside_plane(N[N.size() - 1].id, m_th.c, m_th.d, false);
					make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.d, false);
				}
				else
					temp.push_back({m_th.a, m_th.b, m_th.c, m_th.d, temp.size()});
			}
			else
				temp.push_back({m_th.a, m_th.b, m_th.c, m_th.d, temp.size()});
		}

		TH = temp;
	}
	node_tetrahedron_share_sweep();
}

void mesh::centroid_shift_3d()
{
	for (node &m_n : N)
	{
		pos centroid;
		if (m_n.location == node_location::inside)
		{
			unordered_set<node_id> S;
			vector<node> temp;
			for (const tetrahedron_id th_id : m_n.TH)
			{
				S.insert(TH[th_id].a);
				S.insert(TH[th_id].b);
				S.insert(TH[th_id].c);
				S.insert(TH[th_id].d);
			}

			for (const node_id n_id : S)
				temp.push_back(N[n_id]);

			m_n.p = generate_centroid(temp);
		}
	}
}

/*
void mesh::node_insertion_3d()
{
	for (size_t i = 0; i < number_of_nodes(); i++)
	{
		if (N[i].location == node_location::boundary || N[i].location == node_location::hole)
		{
			if (N[i].tetrahedron_share() > 4) //number to be determined
			{
				unordered_set<node_id> S;
				vector<node> temp;
				pos centroid;
				pair<node_id, node_id> np;
				for (const tetrahedron_id th_id : N[i].TH)
				{
					S.insert(TH[th_id].a);
					S.insert(TH[th_id].b);
					S.insert(TH[th_id].c);
					S.insert(TH[th_id].d);
				}

				for (const node_id n_id : S)
					temp.push_back(N[n_id]);

				centroid = generate_centroid(temp);
				np = corner_pos(N[i]);

				if (distance((N[np.first].p + N[np.second].p) * 0.5, N[i].p) < distance((N[np.first].p + N[np.second].p) * 0.5, centroid))
				{
					N.push_back({centroid, N.size(), node_location::inside, false});

					for (size_t j = 0; j < N[i].triangle_share(); j++)
						triangle_node_change(N[i].T[j], N[i].id, N[N.size() - 1].id);

					for (size_t j = 0; j < N[i].edge_share(); j++)
						edge_node_change(N[i].IE[j], N[i].id, N[N.size() - 1].id);

					N[i].TH.clear();
					N[i].IE.clear();

					make_triangle(N[N.size() - 1].id, N[i].id, np.first);
					make_triangle(N[N.size() - 1].id, N[i].id, np.second);
					make_inside_edge(N[N.size() - 1].id, N[i].id, false);
					make_inside_edge(N[N.size() - 1].id, N[np.first].id, false);
					make_inside_edge(N[N.size() - 1].id, N[np.second].id, false);
				}
			}
		}
	}
}
*/

void mesh::generate_mesh_basic_3d()
{
	plane p1, p2, p3;
	int64_t p_id;
	double min_dist = 1.0e10;
	double temp;
	int k = -1;
	while (number_of_unused_planes(P) != 0)
	{
		for (size_t i = 0; i < number_of_planes(); i++)
		{
			k = -1;
			min_dist = 1.0e10;
			for (size_t j = 0; j < number_of_nodes(); j++)
			{
				if (P[i].availability && N[j].availability && (P[i].a != N[j].id && P[i].b != N[j].id && P[i].c != N[j].id))
				{
					if (left_test(P[i], N[j]) && collinear_test(P[i], N[j]))
					{
						//Have to check
						p1.a = P[i].b;
						p1.b = P[i].a;
						p1.c = N[j].id;

						p2.a = N[j].id;
						p2.b = P[i].c;
						p2.c = P[i].b;

						p3.a = P[i].c;
						p3.b = N[j].id;
						p3.c = P[i].a;

						if (intersection_test(p1, P) && intersection_test(p2, P) && intersection_test(p3, P))
						{
							temp = distance((N[P[i].a].p + N[P[i].b].p + N[P[i].c].p) * 0.333, N[j].p);
							if (temp < min_dist)
							{
								k = j;
								min_dist = temp;
							}
						}
					}
				}
			}

			if (P[i].availability && k > -1)
			{
				//Have to check
				p1.a = P[i].b;
				p1.b = P[i].a;
				p1.c = N[k].id;

				p2.a = N[k].id;
				p2.b = P[i].c;
				p2.c = P[i].b;

				p3.a = P[i].c;
				p3.b = N[k].id;
				p3.c = P[i].a;

				p_id = plane_exists(p1);
				if (p_id != -1)
				{
					P[p_id].availability = false;
					p1.id = p_id;
					disable_common_node(p1, P[i]);
				}

				else
					make_inside_plane(p1.a, p1.b, p1.c, true);

				p_id = plane_exists(p2);
				if (p_id != -1)
				{
					P[p_id].availability = false;
					p2.id = p_id;
					disable_common_node(p2, P[i]);
				}
				else
					make_inside_plane(p2.a, p2.b, p2.c, true);

				p_id = plane_exists(p3);
				if (p_id != -1)
				{
					P[p_id].availability = false;
					p3.id = p_id;
					disable_common_node(p3, P[i]);
				}

				else
					make_inside_plane(p3.a, p3.b, p3.c, true);

				make_tetrahedron(P[i].a, P[i].b, P[i].c, N[k].id);
				P[i].availability = false;
			}
		}
		cout << number_of_unused_planes(P);
		return;
	}
}

} // namespace __mesh3d__