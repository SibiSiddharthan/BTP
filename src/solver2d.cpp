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
	for (const __2d__::triangle t : _T)
		T.push_back(triangle(t));

	generate_ghosts();
	fill_in_data();
	link_triangles();

	cout << T.size() << endl;
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
				T.push_back(__2d__::triangle({m_e.start, m_e.end, N.back().id, T.size(), triangle_type::ghost}));
				T.back().E.push_back(m_e.id);
				N[m_e.start].T.push_back(T.size() - 1);
				N[m_e.end].T.push_back(T.size() - 1);
				N[N.back().id].T.push_back(T.size() - 1);
				m_e.T.push_back(T.back().id);
			}
	}
	ghost_generated = true;
}

void solver::link_triangles()
{
	for (const edge &m_e : E)
	{
		T[m_e.T[0]].S.push_back(m_e.T[1]);
		T[m_e.T[1]].S.push_back(m_e.T[0]);
	}
}

void solver::fill_in_data()
{
	for (triangle &t : T)
	{
		t.centroid = (N[t.a].p + N[t.b].p + N[t.c].p) / 3;

		t.length_a = distance(N[t.b].p, N[t.c].p);
		t.length_b = distance(N[t.a].p, N[t.c].p);
		t.length_c = distance(N[t.a].p, N[t.b].p);

		t.angle_a = angle_between_lines(N[t.b].p, N[t.a].p, N[t.c].p);
		t.angle_b = angle_between_lines(N[t.a].p, N[t.b].p, N[t.c].p);
		t.angle_c = angle_between_lines(N[t.a].p, N[t.c].p, N[t.b].p);
	}
}

void solver::display()
{
	window w(800, 800);
	vector<float> posdata = export_vertex_data();
	vector<GLuint> node_index = export_node_index();
	vector<GLuint> edge_index = export_edge_index();
	vector<GLuint> ghost_edge_index = export_ghost_edge_index();
	vector<color> node_color(N.size());
	vector<color> edge_color(N.size(), colors("red"));
	vector<color> ghost_edge_color(N.size(), colors("turquoise"));
	vector<float> node_size(N.size());
	auto [triangle_index, triangle_color] = visualize_solution();

	pos p;
	float zoom;
	bool draw_ghosts = false;

	for (size_t i = 0; i < N.size(); ++i)
	{
		switch (N[i].location)
		{
		case node_location::boundary:
		{
			node_color[i] = colors("green");
			node_size[i] = 6.0;
		}
		break;

		case node_location::hole:
		{
			node_color[i] = colors("green");
			node_size[i] = 4.0;
		}
		break;

		case node_location::inside:
		{
			node_color[i] = colors("yellow");
			node_size[i] = 3.0;
		}
		break;

		case node_location::outside:
		{
			node_color[i] = colors("purple");
			node_size[i] = 2.0;
		}
		break;
		}
	}

	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_edge(GL_ARRAY_BUFFER, edge_color);
	cb_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_ghost_edge(GL_ARRAY_BUFFER, ghost_edge_color);
	cb_ghost_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_triangle(GL_ARRAY_BUFFER, triangle_color);
	cb_triangle.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer db_node_size(GL_ARRAY_BUFFER, node_size);
	db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_edge(GL_ELEMENT_ARRAY_BUFFER, edge_index),
		ib_ghost_edge(GL_ELEMENT_ARRAY_BUFFER, ghost_edge_index), ib_triangle(GL_ELEMENT_ARRAY_BUFFER, triangle_index);

	vertex_array va_node(GL_POINTS), va_edge(GL_LINES), va_ghost_edge(GL_LINES), va_triangle(GL_TRIANGLES);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &db_node_size, &ib_node});
	va_node.unbind();

	va_edge.bind();
	va_edge.bind_buffer({&vb_pos, &cb_edge, &ib_edge});
	va_edge.unbind();

	va_ghost_edge.bind();
	va_ghost_edge.bind_buffer({&vb_pos, &cb_ghost_edge, &ib_ghost_edge});
	va_ghost_edge.unbind();

	va_triangle.bind();
	va_triangle.bind_buffer({&vb_pos, &cb_triangle, &ib_triangle});
	va_triangle.unbind();

	program prog("src/shaders/display_2d.glsl");

	p = w.get_pos();
	zoom = w.get_zoom();
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
	glm::mat4 scale = glm::scale(trans, glm::vec3(zoom));
	glm::mat4 mvp = scale * trans;

	prog.set_uniform("MVP", uniform_types::MAT4F, (void *)&mvp[0][0]);
	prog.set_uniform("psize", uniform_types::FLOAT, (void *)&zoom);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);

	while (!w.should_close())
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClear(GL_COLOR_BUFFER_BIT);

		va_triangle.draw();
		va_node.draw();
		va_edge.draw();

		p = w.get_pos();
		zoom = w.get_zoom();
		trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
		scale = glm::scale(trans, glm::vec3(zoom));
		mvp = scale * trans;

		prog.update_uniforms();

		ImGui::Begin("Checks");
		ImGui::Checkbox("Draw ghosts", &draw_ghosts);
		ImGui::End();

		if (draw_ghosts)
			va_ghost_edge.draw();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		w.swap_buffers();
		w.poll_events();
	}
}

vector<float> solver::export_vertex_data() const
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

vector<uint32_t> solver::export_node_index() const
{
	vector<uint32_t> I;
	for (size_t i = 0; i < N.size(); ++i)
		if (N[i].location != __2d__::node_location::outside)
			I.push_back(N[i].id);

	return I;
}

vector<uint32_t> solver::export_edge_index() const
{
	vector<uint32_t> I(E.size() * 2);
	size_t k = 0;
	for (size_t i = 0; i < E.size() * 2; i += 2)
	{
		k = i / 2;
		I[i + 0] = uint32_t(E[k].start);
		I[i + 1] = uint32_t(E[k].end);
	}
	return I;
}

vector<uint32_t> solver::export_ghost_edge_index() const
{
	vector<uint32_t> I;
	for (const triangle &t : T)
		if (t.type == __2d__::triangle_type::ghost)
		{
			I.push_back(uint32_t(t.a));
			I.push_back(uint32_t(t.b));

			I.push_back(uint32_t(t.b));
			I.push_back(uint32_t(t.c));

			I.push_back(uint32_t(t.c));
			I.push_back(uint32_t(t.a));
		}

	return I;
}

void solver::apply_boundary_condition_default()
{
	for (triangle &t : T)
		if (t.type == __2d__::triangle_type::ghost)
		{
			//cout<<t.S.size();
			if (N[t.a].location == __2d__::node_location::hole ||
				N[t.b].location == __2d__::node_location::hole ||
				N[t.c].location == __2d__::node_location::hole)
				t.value = -T[t.S[0]].value;

			else if (N[t.a].location == __2d__::node_location::boundary ||
					 N[t.b].location == __2d__::node_location::boundary ||
					 N[t.c].location == __2d__::node_location::boundary)
				t.value = 2 - T[t.S[0]].value;
			//printf("%lf %lf\n",t.value,t.S)
		}
}

void solver::solve()
{

	for (int i = 0; i < 5; ++i)
	{
		apply_boundary_condition_default();
		for (triangle &t : T)
			if (t.type == __2d__::triangle_type::domain)
			{
				t.value = (T[t.S[0]].value + T[t.S[1]].value + T[t.S[2]].value) / 3;
				//printf("%lf %lf - %llu %lf - %llu %lf - %llu %d\n",t.value,T[t.S[0]].value,t.S[0],T[t.S[1]].value,t.S[1],T[t.S[2]].value,t.S[2],i);
			}
	}

	update_node_values();
}

tuple<vector<uint32_t>, vector<color>> solver::visualize_solution()
{
	vector<uint32_t> I;
	vector<color> C;
	for (const triangle &t : T)
		if (t.type == __2d__::triangle_type::domain)
		{
			I.push_back(uint32_t(t.a));
			I.push_back(uint32_t(t.b));
			I.push_back(uint32_t(t.c));
		}
	for (const node &n : N)
		if (n.location != __2d__::node_location::outside)
			C.push_back({0.0f, float(n.value), 1 - float(n.value)});

	return make_tuple(I, C);
}

void solver::update_node_values()
{
	for (node &n : N)
	{
		if (n.location == __2d__::node_location::boundary)
			n.value = 1;
		else if (n.location == __2d__::node_location::hole)
			n.value = 0;
		else if (n.location == __2d__::node_location::inside)
		{
			double avg = 0;
			for (triangle_id t_id : n.T)
				avg += T[t_id].value;
			n.value = avg / n.T.size();
		}

		//cout<<n.value<<endl;
	}
}

} // namespace solver
} // namespace __2d__