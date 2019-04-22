#include "solver2d.h"

using namespace std;

namespace __2d__
{

namespace solver
{
void solver::import_mesh(const mesh &M)
{
	auto [_N, _E, _T] = M.export_mesh();
	N = _N;
	E = _E;
	for (const __2d__::triangle t:	_T)
		T.push_back(triangle(t));

	generate_ghosts();
	link_triangles();
}

void solver::generate_ghosts()
{
	auto generate_ghost_point = [this](triangle_id t_id, node_id n_id) -> pos {
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
	};

	auto vertex_opposite_to_triangle_edge = [this](const triangle &t, const edge &e) -> node_id {
		if (N[t.a].id != e.start && N[t.a].id != e.end)
			return t.a;
		else if (N[t.b].id != e.start && N[t.b].id != e.end)
			return t.b;
		else //if (N[T[t_id].c].id != e.start && N[T[t_id].c].id != e.end)
			return t.c;
	};

	if (!ghost_generated)
	{
		//Need to guarantee
		//edge_triangle_share_sweep();

		for (edge &m_e : E)
			if (m_e.location == edge_location::boundary)
			{
				node_id n_id = vertex_opposite_to_triangle_edge(T[m_e.T[0]], m_e);
				pos p = generate_ghost_point(m_e.T[0], n_id);
				N.push_back({p, N.size(), node_location::outside, false});

				//make_triangle(m_e.start, m_e.end, N[N.size() - 1].id, triangle_type::ghost);
				T.push_back({m_e.start, m_e.end, N.back().id, T.size(), triangle_type::ghost});
				N[m_e.start].T.push_back(T.size() - 1);
				N[m_e.end].T.push_back(T.size() - 1);
				N[N.back().id].T.push_back(T.size() - 1);
			}
	}
	ghost_generated = true;
}

} // namespace solver
} // namespace __2d__