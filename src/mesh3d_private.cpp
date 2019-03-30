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
		unordered_set<tetrahedron_id> tetrahedron_track;
		for (const tetrahedron_id th_id : N[m_p.a].TH)
			tetrahedron_track.insert(th_id);

		for (const tetrahedron_id th_id : N[m_p.b].TH)
		{
			if (tetrahedron_track.find(th_id) != tetrahedron_track.end())
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

void mesh::make_inside_plane(const node_id a, const node_id b, const node_id c,bool availability)
{
	P.push_back({a, b, c, P.size(), plane_location::inside, availability});

	N[a].IP.push_back(P.size() - 1);
	N[b].IP.push_back(P.size() - 1);
	N[c].IP.push_back(P.size() - 1);
}

void mesh::make_tetrahedron(const node_id a, const node_id b, const node_id c,const node_id d, tetrahedron_type tp)
{
	TH.push_back({a, b, c,d, TH.size(), tp});

	N[a].TH.push_back(TH.size() - 1);
	N[b].TH.push_back(TH.size() - 1);
	N[c].TH.push_back(TH.size() - 1);
	N[d].TH.push_back(TH.size() - 1);
}

void mesh::disable_common_node(const plane& p1 ,const plane& p2)
{
	unordered_set<node_id> s;
	s.insert(p1.a);
	s.insert(p1.b);
	s.insert(p1.c);
	
	if(s.find(p2.a)!=s.end())
		N[p2.a].availability = false;
	if(s.find(p2.b)!=s.end())
		N[p2.b].availability = false;
	if(s.find(p2.c)!=s.end())
		N[p2.c].availability = false;

}

int64_t mesh::plane_exists(const plane& p)
{
	for (const plane &m_p : P)
	{
		vector<plane_id> a = {p.a,p.b,p.c},b = {m_p.a,m_p.b,m_p.c};
		sort(a.begin(),a.end());
		sort(b.begin(),b.end());

		if(a[0] == b[0] && a[1] == b[1] && a[2]==b[2])
			return m_p.id;

	}
	return -1;
}

}