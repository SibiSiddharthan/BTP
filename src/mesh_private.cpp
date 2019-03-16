#include "mesh.h"
/*
This File contains the definitions of the private member of functions of the mesh
class
*/

using namespace std;

/*Creates a trinagle and updates the triangle_share()s of it's nodes
a,b,c are node pointers that form the triangle
n is the number of */
//updated on 23/8/18
void mesh::make_triangle(const node_id a, const node_id b, const node_id c, triangle_type tp)
{
	T.push_back({a, b, c, T.size(), tp});

	N[a].T.push_back(T.size() - 1);
	N[b].T.push_back(T.size() - 1);
	N[c].T.push_back(T.size() - 1);

	//T[T.size()-1].type = tp;
	//printf("%s\n",(T[T.size()-1].type==triangle_type::domain)?"domain":"ghost");
}

void mesh::make_inside_edge(const node_id start, const node_id end, bool availability)
{
	E.push_back({start, end, E.size(), edge_location::inside, availability});

	N[start].IE.push_back(E.size() - 1);
	N[end].IE.push_back(E.size() - 1);
}


//updated on 23/8/18
double mesh::avg_area_of_triangles()
{
	double res = 0;
	for (const mesh_triangle &m_t : T)
		res += triangle_area(m_t);

	return res / number_of_triangles();
}

//updated on 23/8/18
double mesh::avg_area_of_triangles_near_boundary(node_location loc)
{
	double res = 0;
	uint64_t count = 0;
	for (const mesh_triangle &m_t : T)
		if (N[m_t.a].location == loc || N[m_t.b].location == loc || N[m_t.c].location == loc)
		{
			res += triangle_area(m_t);
			++count;
		}
	return res / count;
}

double mesh::avg_volume_of_tetrahedrons()
{
	double res = 0;
	uint64_t count = 0;
	for (const tetrahedron &m_te : TH)
	{
		res += tetrahedron_volume(m_te);
		++count;
	}
	return res / count;
}

double mesh::avg_volume_of_tetrahedrons_near_boundary(node_location loc)
{
	double res = 0;
	uint64_t count = 0;
	for (const tetrahedron &m_te : TH)
		if (N[m_te.a].location == loc || N[m_te.b].location == loc || N[m_te.c].location == loc || N[m_te.d].location == loc)
		{
			res += tetrahedron_volume(m_te);
			++count;
		}
	return res / count;
}

//updated on 23/8/18
void mesh::node_triangle_share_sweep()
{
	for (node &m_n : N)
		m_n.T.clear();

	for (const mesh_triangle &m_t : T)
	{
		N[m_t.a].T.push_back(m_t.id);
		N[m_t.b].T.push_back(m_t.id);
		N[m_t.c].T.push_back(m_t.id);
	}
}

void mesh::node_edge_share_sweep()
{
	for (node &m_n : N)
		m_n.IE.clear();

	for (const edge &m_e : E)
	{
		if (m_e.location == edge_location::inside)
		{
			N[m_e.start].IE.push_back(m_e.id);
			N[m_e.end].IE.push_back(m_e.id);
		}
	}
}

void mesh::edge_triangle_share_sweep()
{
	for (edge &m_e : E)
		m_e.T.clear();

	for (edge &m_e : E)
	{
		uint64_t count = 0;
		unordered_set<triangle_id> triangle_track;
		for (const triangle_id t_id : N[m_e.start].T)
			triangle_track.insert(t_id);

		for (const triangle_id t_id : N[m_e.end].T)
		{
			if (triangle_track.find(t_id) != triangle_track.end())
			{
				m_e.T.push_back(t_id);
				++count;
			}

			if (count == 2 && m_e.location == edge_location::inside)
				break;
			else if (count == 1 && m_e.location == edge_location::boundary)
				break;
		}
	}
}

void mesh::triangle_edge_share_sweep()
{
	for (mesh_triangle &m_t : T)
		m_t.E.clear();

	for (const edge &m_e : E)
	{
		for (const triangle_id t_id : m_e.T)
			T[t_id].E.push_back(m_e.id);
	}
}

//Helper Functions
int64_t mesh::edge_exists(const edge &e)
{
	for (const edge &m_e : E)
	{
		if ((N[m_e.start] == N[e.start] && N[m_e.end] == N[e.end]) || (N[m_e.start] == N[e.end] && N[m_e.end] == N[e.start]))
		{
			return m_e.id;
		}
	}
	return -1;
}

//updated on 23/8/18
//When a triangle is formed between two adjacent edges their common node is diabled
//This function is only used by generate_mesh_basic
void mesh::disable_common_node(const edge &a, const edge &b)
{
	if (N[a.start] == N[b.start])
		N[b.start].availability = false;
	else if (N[a.end] == N[b.start])
		N[b.start].availability = false;
	else if (N[a.start] == N[b.end])
		N[b.end].availability = false;
	else if (N[a.end] == N[b.end])
		N[b.end].availability = false;
}

//updated on 23/8/18
//Returns a pair of nodes that are on either side of the node in consideration on the boundary or on thehole
//This function is only used by node_insertion
pair<node_id, node_id> mesh::corner_pos(const node &n)
{
	if (n == N[E[n.BE[0]].start])
		return std::make_pair(E[n.BE[0]].end, E[n.BE[1]].start);

	else //if (n == N[E[n.BE[0]].end])
		return std::make_pair(E[n.BE[0]].start, E[n.BE[1]].end);
}

//updated on 23/8/18
/*Generates a ghost point by reflecting the third vertex(not on boundary or on hole) of the triangle on the boundary
about it's opposite side*/
//This function is only used by generate_ghosts
pos mesh::generate_ghost_point(const triangle_id t_id, const node_id n_id)
{
	pos res;
	pos p = N[n_id].p;
	double angle;
	if (T[t_id].a == n_id)
	{
		angle = line_inclination_absolute({N[T[t_id].b].p, N[T[t_id].c].p});
		p -= N[T[t_id].b].p;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res += N[T[t_id].b].p;
	}

	else if (T[t_id].b == n_id)
	{
		angle = line_inclination_absolute({N[T[t_id].c].p, N[T[t_id].a].p});
		p -= N[T[t_id].c].p;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res += N[T[t_id].c].p;
	}

	else //if (T[t_id].c == n_id)
	{
		angle = line_inclination_absolute({N[T[t_id].a].p, N[T[t_id].b].p});
		p -= N[T[t_id].a].p;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res += N[T[t_id].a].p;
	}

	return res;
}

pos mesh::generate_centroid(const std::vector<node> &m_N)
{
	pos centroid;
	for (const node &n : m_N)
		centroid += n.p;

	return centroid / m_N.size();
}

//updated on 23/8/18
//Finds the vertex opposite to the common edge
//This function is only used by edge_swap
const node_id mesh::vertex_opposite_to_triangle_edge(const mesh_triangle &t, const edge &e)
{
	if (N[t.a].id != e.start && N[t.a].id != e.end)
		return t.a;
	else if (N[t.b].id != e.start && N[t.b].id != e.end)
		return t.b;
	else //if (N[T[t_id].c].id != e.start && N[T[t_id].c].id != e.end)
		return t.c;
}

void mesh::triangle_node_change(const triangle_id t_id, const node_id fn_id, const node_id tn_id)
{
	if (T[t_id].a == fn_id)
	{
		T[t_id].a = tn_id;
		N[tn_id].T.push_back(t_id);
	}

	else if (T[t_id].b == fn_id)
	{
		T[t_id].b = tn_id;
		N[tn_id].T.push_back(t_id);
	}

	else //if (T[t_id].c == fn_id)
	{
		T[t_id].c = tn_id;
		N[tn_id].T.push_back(t_id);
	}
}

void mesh::edge_node_change(const edge_id e_id, const node_id fn_id, const node_id tn_id)
{
	if (E[e_id].start == fn_id)
	{
		E[e_id].start = tn_id;
		N[tn_id].IE.push_back(e_id);
	}

	else //if(E[e_id].end == fn_id)
	{
		E[e_id].end = tn_id;
		N[tn_id].IE.push_back(e_id);
	}
}

void mesh::update_pdata()
{
	pdata.erase(pdata.begin(), pdata.end());
	pdata.reserve(3 * N.size());
	if (N.size() != 0)
	{
		for (const node &n : N)
		{
			pdata.push_back((float)n.p.x);
			pdata.push_back((float)n.p.y);
			pdata.push_back((float)n.p.z);
		}
	}
}

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

	for (const mesh_plane &m_p : P)
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
	for (mesh_plane &m_p : P)
		m_p.TH.clear();

	for (mesh_plane &m_p : P)
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

	for (const mesh_plane &m_p : P)
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

void mesh::disable_common_node(const mesh_plane& p1 ,const mesh_plane& p2)
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

int64_t mesh::plane_exists(const mesh_plane& p)
{
	for (const mesh_plane &m_p : P)
	{
		vector<plane_id> a = {p.a,p.b,p.c},b = {m_p.a,m_p.b,m_p.c};
		sort(a.begin(),a.end());
		sort(b.begin(),b.end());

		if(a[0] == b[0] && a[1] == b[1] && a[2]==b[2])
			return m_p.id;

	}
	return -1;
}
