#include "mesh3d.h"

using namespace std;

namespace __3d__
{
void mesh::node_tetrahedron_share_sweep()
{
	for (node &m_n : N)
		m_n.TH.clear();

	for (const tetrahedron &m_th : TH)
	{
		N[m_th.a].TH.push_back(m_th.id);
		N[m_th.b].TH.push_back(m_th.id);
		N[m_th.c].TH.push_back(m_th.id);
		N[m_th.d].TH.push_back(m_th.id);
	}
}

void mesh::node_plane_share_sweep()
{
	for (node &m_n : N)
		m_n.IP.clear();

	for (const plane &m_p : P)
	{
		if (m_p.location == plane_location::inside)
		{
			N[m_p.a].IP.push_back(m_p.id);
			N[m_p.b].IP.push_back(m_p.id);
			N[m_p.c].IP.push_back(m_p.id);
		}
	}
}

void mesh::plane_tetrahedron_share_sweep()
{
	for (plane &m_p : P)
		m_p.TH.clear();

	for (plane &m_p : P)
	{
		uint64_t count = 0;
		unordered_set<tetrahedron_id> tetrahedron_track_a,tetrahedron_track_b;
		for (const tetrahedron_id th_id : N[m_p.a].TH)
			tetrahedron_track_a.insert(th_id);
		for (const tetrahedron_id th_id : N[m_p.b].TH)
			tetrahedron_track_b.insert(th_id);

		for (const tetrahedron_id th_id : N[m_p.c].TH)
		{
			if (tetrahedron_track_a.find(th_id) != tetrahedron_track_a.end() && tetrahedron_track_b.find(th_id) != tetrahedron_track_b.end())
			{
				m_p.TH.push_back(th_id);
				++count;
			}

			if (count == 2 && m_p.location == plane_location::inside)
				break;
			else if (count == 1 && m_p.location == plane_location::boundary)
				break;
		}
	}

	//for (plane &m_p : P)
		//cout<<m_p.TH.size()<<endl;
}

void mesh::tetrahedron_plane_share_sweep()
{
	for (tetrahedron &m_th : TH)
		m_th.P.clear();

	for (const plane &m_p : P)
	{
		for (const tetrahedron_id th_id : m_p.TH)
			TH[th_id].P.push_back(m_p.id);
	}
}

void mesh::make_inside_plane(const node_id a, const node_id b, const node_id c, bool availability, const node_id check)
{
	P.push_back({a, b, c, P.size(), plane_location::inside, availability});
	P.back().normal = cross(N[a].p - N[c].p, N[b].p - N[c].p);
	P.back().normal /= P.back().normal.norm();

	if(left_test(P.back(),N[check]))
		P.back().normal *= -1.0;

	N[a].IP.push_back(P.size() - 1);
	N[b].IP.push_back(P.size() - 1);
	N[c].IP.push_back(P.size() - 1);
}

void mesh::make_tetrahedron(const node_id a, const node_id b, const node_id c, const node_id d, tetrahedron_type tp)
{
	TH.push_back({a, b, c, d, TH.size(), tp});

	N[a].TH.push_back(TH.size() - 1);
	N[b].TH.push_back(TH.size() - 1);
	N[c].TH.push_back(TH.size() - 1);
	N[d].TH.push_back(TH.size() - 1);
}

void mesh::disable_common_node(const plane &p1, const plane &p2)
{
	unordered_set<node_id> s;
	s.insert(p1.a);
	s.insert(p1.b);
	s.insert(p1.c);

	if (s.find(p2.a) != s.end())
		N[p2.a].availability = false;
	if (s.find(p2.b) != s.end())
		N[p2.b].availability = false;
	if (s.find(p2.c) != s.end())
		N[p2.c].availability = false;
}

int64_t mesh::plane_exists(const plane &p)
{
	for (const plane &m_p : P)
	{
		vector<plane_id> a = {p.a, p.b, p.c}, b = {m_p.a, m_p.b, m_p.c};
		sort(a.begin(), a.end());
		sort(b.begin(), b.end());

		if (a[0] == b[0] && a[1] == b[1] && a[2] == b[2])
			return m_p.id;
	}
	return -1;
}

vector<float> mesh::export_vertex_data() const
{
	vector<float> POS(3 * N.size());
	size_t k = 0;
	for (size_t i = 0; i < 3 * N.size(); i += 3)
	{
		k = i / 3;
		POS[i + 0] = float(N[k].p.x);
		POS[i + 1] = float(N[k].p.y);
		POS[i + 2] = float(N[k].p.z);
	}
	return POS;
}

vector<uint32_t> mesh::export_node_index() const
{
	vector<uint32_t> I(N.size());
	for (size_t i = 0; i < N.size(); ++i)
		I[i] = N[i].id;
	return I;
}

vector<uint32_t> mesh::export_plane_index() const
{
	vector<uint32_t> I(P.size() * 3);
	size_t k = 0;
	for (size_t i = 0; i < P.size() * 3; i += 3)
	{
		k = i / 3;
		I[i + 0] = uint32_t(P[k].a);
		I[i + 1] = uint32_t(P[k].b);
		I[i + 2] = uint32_t(P[k].c);
	}
	return I;
}

vector<uint32_t> mesh::export_plane_edge_index() const
{
	vector<uint32_t> I(P.size() * 6);
	size_t k = 0;

	for (size_t i = 0; i < 6 * P.size(); i += 6)
	{
		k = i / 6;
		I[i + 0] = uint32_t(P[k].a);
		I[i + 1] = uint32_t(P[k].b);

		I[i + 2] = uint32_t(P[k].a);
		I[i + 3] = uint32_t(P[k].c);

		I[i + 4] = uint32_t(P[k].b);
		I[i + 5] = uint32_t(P[k].c);
	}

	return I;
}

tuple<vector<float>, vector<uint32_t>> mesh::export_normals() const
{
	vector<float> NPOS(P.size() * 6);
	vector<uint32_t> I(P.size() * 2);
	size_t k = 0;

	for (size_t i = 0; i < 6 * P.size(); i += 6)
	{
		k = i / 6;
		pos centroid = (N[P[k].a].p + N[P[k].b].p + N[P[k].c].p) * 0.3333;
		pos normal_ext = centroid + P[k].normal * 0.1;

		NPOS[i + 0] = (float)(centroid.x);
		NPOS[i + 1] = (float)(centroid.y);
		NPOS[i + 2] = (float)(centroid.z);
		I[int((i + 2) / 3)] = uint32_t((i + 2) / 3);

		NPOS[i + 3] = (float)(normal_ext.x);
		NPOS[i + 4] = (float)(normal_ext.y);
		NPOS[i + 5] = (float)(normal_ext.z);
		I[int((i + 5) / 3)] = uint32_t((i + 5) / 3);
	}

	return make_tuple(NPOS,I);
}

} // namespace __3d__