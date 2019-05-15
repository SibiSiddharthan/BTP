#include "model3d.h"
#include "stl_reader.h"
#include "gl_abstraction.h"
#include "model2d.h"
#include "mesh2d.h"

using namespace std;

namespace __3d__
{
void model::stl_read(const string filepath)
{
	ifstream f;
	vector<::plane> _P;
	read_stl(f, filepath, _P);

	unordered_map<pos, uint64_t> s;
	uint64_t n = 0;
	for (const ::plane &p : _P)
	{
		if (s.find(p.a) == s.end())
			s.insert({p.a, n++});

		if (s.find(p.b) == s.end())
			s.insert({p.b, n++});

		if (s.find(p.c) == s.end())
			s.insert({p.c, n++});
	}

	N = vector<node>(s.size());
	for (const auto &[m_pos, m_id] : s)
	{
		N[m_id] = {m_pos, m_id, node_location::boundary};
	}

	for (const ::plane &p : _P)
	{
		P.push_back({s[p.a], s[p.b], s[p.c], p.normal, P.size()});
	}
}

void model::display()
{
	window w(800, 800);

	vector<float> posdata = export_vertex_data();
	vector<GLuint> node_index = export_node_index();
	vector<color> node_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_index = export_plane_index();
	vector<color> plane_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_edge_index = export_plane_edge_index();
	vector<color> plane_edge_color(number_of_nodes(), colors("red"));
	auto [normalpos, normals] = export_normals();
	vector<color> normal_color(2 * number_of_planes(), colors("blue"));

	bool draw_normals = false;
	ImGuiIO &io = ImGui::GetIO();
	pos p;
	float rotx, roty;

	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer vb_normal(GL_ARRAY_BUFFER, normalpos);
	vb_normal.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_plane(GL_ELEMENT_ARRAY_BUFFER, plane_index),
		ib_plane_edge(GL_ELEMENT_ARRAY_BUFFER, plane_edge_index);

	data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_plane_edge(GL_ARRAY_BUFFER, plane_edge_color);
	cb_plane_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_plane(GL_ARRAY_BUFFER, plane_color);
	cb_plane.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_normal(GL_ARRAY_BUFFER, normal_color);
	cb_normal.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer ib_normal(GL_ELEMENT_ARRAY_BUFFER, normals);

	vertex_array va_node(GL_POINTS), va_plane(GL_TRIANGLES), va_plane_edge(GL_LINES), va_normals(GL_LINES);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &ib_node});
	va_node.unbind();

	va_plane.bind();
	va_plane.bind_buffer({&vb_pos, &cb_plane, &ib_plane});
	va_plane.unbind();

	va_plane_edge.bind();
	va_plane_edge.bind_buffer({&vb_pos, &cb_plane_edge, &ib_plane_edge});
	va_plane_edge.unbind();

	va_normals.bind();
	va_normals.bind_buffer({&vb_normal, &cb_normal, &ib_normal});
	va_normals.unbind();

	p = w.get_pos();
	rotx = w.get_rotx();
	roty = w.get_roty();
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

	program prog("src/shaders/display_3d.glsl");
	prog.set_uniform("MVP", uniform_types::MAT4F, &View[0][0]);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	while (!w.should_close())
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		va_node.draw();
		va_plane.draw();
		va_plane_edge.draw();

		if (draw_normals)
			va_normals.draw();

		ImGui::Begin("model");
		ImGui::Checkbox("draw normals", &draw_normals);
		ImGui::End();

		p = w.get_pos();
		rotx = w.get_rotx();
		roty = w.get_roty();
		ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
		ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
		View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

		prog.update_uniforms();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		w.swap_buffers();
		w.poll_events();
	}
}

vector<float> model::export_vertex_data() const
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

vector<uint32_t> model::export_node_index() const
{
	vector<uint32_t> I(N.size());
	for (size_t i = 0; i < N.size(); ++i)
		I[i] = N[i].id;
	return I;
}

vector<uint32_t> model::export_plane_index() const
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

vector<uint32_t> model::export_plane_edge_index() const
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

tuple<vector<float>, vector<uint32_t>> model::export_normals() const
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

	return make_tuple(NPOS, I);
}

void model::add_boundary_sphere(double radius, double dx)
{
	__2d__::model circle;
	circle.add_boundary_circle(radius, dx);
	__2d__::mesh mesh_circle;
	mesh_circle.import_2d(circle);
	mesh_circle.generate_mesh_full();

	auto [_N, __, _T] = mesh_circle.export_mesh();

	unordered_map<node_id, node_id> top, bottom;

	//z = 0 plane
	for (const __2d__::node &n : _N)
		if (n.location == __2d__::node_location::boundary)
		{
			N.push_back({n.p, N.size(), __3d__::node_location::boundary});
			top.insert({n.id, N.back().id});
			bottom.insert({n.id, N.back().id});
		}

	//Top and bottom hemisphere
	for (__2d__::node n : _N)
		if (n.location == __2d__::node_location::inside)
		{
			//Top
			n.p.z = sqrt(radius * radius - (n.p.x * n.p.x + n.p.y * n.p.y));
			N.push_back({n.p, N.size(), __3d__::node_location::boundary});
			top.insert({n.id, N.back().id});

			//Bottom
			n.p.z = -sqrt(radius * radius - (n.p.x * n.p.x + n.p.y * n.p.y));
			N.push_back({n.p, N.size(), __3d__::node_location::boundary});
			bottom.insert({n.id, N.back().id});
		}

	for (const auto &t : _T)
	{
		pos p1, p2, p3;
		p1 = _N[t.a].p;
		p2 = _N[t.b].p;
		p3 = _N[t.c].p;
		if (left_test_2d({p1, p2}, p3))
		{
			P.push_back({top[t.a], top[t.b], top[t.c], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();

			P.push_back({bottom[t.a], bottom[t.c], bottom[t.b], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();
		}

		else
		{
			P.push_back({top[t.a], top[t.c], top[t.b], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();

			P.push_back({bottom[t.a], bottom[t.b], bottom[t.c], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();
		}
	}
}

void model::add_hole_sphere(pos centre, double radius, double dx)
{
	__2d__::model circle;
	circle.add_boundary_circle(radius, dx);
	__2d__::mesh mesh_circle;
	mesh_circle.import_2d(circle);
	mesh_circle.generate_mesh_full();

	auto [_N, __, _T] = mesh_circle.export_mesh();

	unordered_map<node_id, node_id> top, bottom;

	//z = 0 plane
	for (const __2d__::node &n : _N)
		if (n.location == __2d__::node_location::boundary)
		{
			N.push_back({n.p + centre, N.size(), __3d__::node_location::boundary});
			top.insert({n.id, N.back().id});
			bottom.insert({n.id, N.back().id});
		}

	//Top and bottom hemisphere
	for (__2d__::node n : _N)
		if (n.location == __2d__::node_location::inside)
		{
			//Top
			n.p.z = sqrt(radius * radius - (n.p.x * n.p.x + n.p.y * n.p.y));
			N.push_back({n.p + centre, N.size(), __3d__::node_location::boundary});
			top.insert({n.id, N.back().id});

			//Bottom
			n.p.z = -sqrt(radius * radius - (n.p.x * n.p.x + n.p.y * n.p.y));
			N.push_back({n.p + centre, N.size(), __3d__::node_location::boundary});
			bottom.insert({n.id, N.back().id});
		}

	for (const auto &t : _T)
	{
		pos p1, p2, p3;
		p1 = _N[t.a].p;
		p2 = _N[t.b].p;
		p3 = _N[t.c].p;
		if (!left_test_2d({p1, p2}, p3))
		{
			P.push_back({top[t.a], top[t.b], top[t.c], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();

			P.push_back({bottom[t.a], bottom[t.c], bottom[t.b], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();
		}

		else
		{
			P.push_back({top[t.a], top[t.c], top[t.b], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();

			P.push_back({bottom[t.a], bottom[t.b], bottom[t.c], P.size(), __3d__::plane_location::boundary});
			P.back().normal = cross(N[P.back().a].p - N[P.back().c].p, N[P.back().b].p - N[P.back().c].p);
			P.back().normal /= P.back().normal.norm();
		}
	}
}

void model::add_boundary_cube(double length, double dx)
{
	__2d__::model square;
	square.add_boundary_square(length, dx);
	__2d__::mesh mesh_square;
	mesh_square.import_2d(square);
	mesh_square.generate_mesh_full();

	auto [_N, __, _T] = mesh_square.export_mesh();

	unordered_map<pos, node_id> um;
	unordered_map<node_id, node_id> top, bottom, front, back, left, right;

	for (const __2d__::node &n : _N)
		if (n.location == __2d__::node_location::boundary)
		{
			pos temp;
			//front
			temp = n.p + pos(0, 0, length * 0.5);
			if (um.find(temp) == um.end())
			{
				N.push_back({temp, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			front[n.id] = um[temp];
			
			//back
			temp = n.p + pos(0, 0, -length * 0.5);
			if (um.find(temp) == um.end())
			{
				N.push_back({temp, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			back[n.id] = um[temp];
			

			//left
			temp = {-length * 0.5, n.p.y, n.p.x};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			left[n.id] = um[temp];

			//right
			temp = {length * 0.5, n.p.y, -n.p.x};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			right[n.id] = um[temp];

			//top
			temp = {n.p.x, length * 0.5, -n.p.y};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			top[n.id] = um[temp];

			//bottom
			temp = {n.p.x, -length * 0.5, n.p.y};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			bottom[n.id] = um[temp];
		}

	for (const __2d__::node &n : _N)
		if (n.location == __2d__::node_location::inside)
		{
			pos temp;
			//front
			temp = n.p + pos(0, 0, length * 0.5);
			N.push_back({temp, N.size(), __3d__::node_location::boundary});
			front[n.id] = N.back().id;
			
			//back
			temp = n.p + pos(0, 0, length * -0.5);
			N.push_back({temp, N.size(), __3d__::node_location::boundary});
			back[n.id] = N.back().id;

			//left
			temp = {-length * 0.5, n.p.y, n.p.x};
			N.push_back({temp, N.size(), __3d__::node_location::boundary});
			left[n.id] = N.back().id;

			//right
			temp = {length * 0.5, n.p.y, -n.p.x};
			N.push_back({temp, N.size(), __3d__::node_location::boundary});
			right[n.id] = N.back().id;

			//top
			temp = {n.p.x, length * 0.5, -n.p.y};
			N.push_back({temp, N.size(), __3d__::node_location::boundary});
			top[n.id] = N.back().id;

			//bottom
			temp = {n.p.x, -length * 0.5, n.p.y};
			N.push_back({temp, N.size(), __3d__::node_location::boundary});
			bottom[n.id] = N.back().id;
		}

	
	for (const auto &t : _T)
	{
		P.push_back({front[t.a], front[t.b], front[t.c], {0, 0, 1.0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({back[t.a], back[t.b], back[t.c], {0, 0, -1.0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({left[t.a], left[t.b], left[t.c], {-1.0, 0, 0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({right[t.a], right[t.b], right[t.c], {1.0, 0, 0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({top[t.a], top[t.b], top[t.c], {0, 1.0, 0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({bottom[t.a], bottom[t.b], bottom[t.c], {0, -1.0, 0}, P.size(), __3d__::plane_location::boundary});
	}
}

void model::add_hole_cube(pos centre, double length, double dx)
{
	__2d__::model square;
	square.add_boundary_square(length, dx);
	__2d__::mesh mesh_square;
	mesh_square.import_2d(square);
	mesh_square.generate_mesh_full();

	auto [_N, __, _T] = mesh_square.export_mesh();

	unordered_map<pos, node_id> um;
	unordered_map<node_id, node_id> top, bottom, front, back, left, right;

	for (const __2d__::node &n : _N)
		if (n.location == __2d__::node_location::boundary)
		{
			pos temp;
			//front
			temp = n.p + pos(0, 0, length * 0.5);
			if (um.find(temp) == um.end())
			{
				N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			front[n.id] = um[temp];
			
			//back
			temp = n.p + pos(0, 0, -length * 0.5);
			if (um.find(temp) == um.end())
			{
				N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			back[n.id] = um[temp];
			

			//left
			temp = {-length * 0.5, n.p.y, n.p.x};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			left[n.id] = um[temp];

			//right
			temp = {length * 0.5, n.p.y, -n.p.x};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			right[n.id] = um[temp];

			//top
			temp = {n.p.x, length * 0.5, -n.p.y};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			top[n.id] = um[temp];

			//bottom
			temp = {n.p.x, -length * 0.5, n.p.y};
			if (um.find(temp) == um.end())
			{
				N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
				um[temp] = N.back().id;
			}
			bottom[n.id] = um[temp];
		}

	for (const __2d__::node &n : _N)
		if (n.location == __2d__::node_location::inside)
		{
			pos temp;
			//front
			temp = n.p + pos(0, 0, length * 0.5);
			N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
			front[n.id] = N.back().id;
			
			//back
			temp = n.p + pos(0, 0, length * -0.5);
			N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
			back[n.id] = N.back().id;

			//left
			temp = {-length * 0.5, n.p.y, n.p.x};
			N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
			left[n.id] = N.back().id;

			//right
			temp = {length * 0.5, n.p.y, -n.p.x};
			N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
			right[n.id] = N.back().id;

			//top
			temp = {n.p.x, length * 0.5, -n.p.y};
			N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
			top[n.id] = N.back().id;

			//bottom
			temp = {n.p.x, -length * 0.5, n.p.y};
			N.push_back({temp + centre, N.size(), __3d__::node_location::boundary});
			bottom[n.id] = N.back().id;
		}

	
	for (const auto &t : _T)
	{
		P.push_back({front[t.a], front[t.b], front[t.c], {0, 0, -1.0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({back[t.a], back[t.b], back[t.c], {0, 0, 1.0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({left[t.a], left[t.b], left[t.c], {1.0, 0, 0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({right[t.a], right[t.b], right[t.c], {-1.0, 0, 0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({top[t.a], top[t.b], top[t.c], {0, -1.0, 0}, P.size(), __3d__::plane_location::boundary});
		P.push_back({bottom[t.a], bottom[t.b], bottom[t.c], {0, 1.0, 0}, P.size(), __3d__::plane_location::boundary});
	}
}

} // namespace __3d__