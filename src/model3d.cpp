#include "model3d.h"
#include "stl_reader.h"
using namespace std;

namespace __3d__
{
void model::stl_read(const string filepath)
{
	ifstream f;
	vector<::plane> _P;
	read_stl(f, "tests/sphere1.stl", _P);

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

/*
void model::display()
{

	vector<float> posdata(3 * number_of_nodes());
	vector<GLuint> node_index(number_of_nodes());
	vector<color> node_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_index(3 * number_of_planes());
	vector<color> plane_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_edge_index(6 * number_of_planes());
	vector<color> plane_edge_color(number_of_nodes(), colors("red"));
	vector<float> normalpos(6 * number_of_planes());
	vector<GLuint> normals(2 * number_of_planes());
	vector<color> normal_color(2 * number_of_planes(), colors("blue"));

	uint64_t k = 0;

	for (size_t i = 0; i < 3 * number_of_nodes(); i += 3)
	{
		k = i / 3;
		posdata[i + 0] = (float)N[k].p.x;
		posdata[i + 1] = (float)N[k].p.y;
		posdata[i + 2] = (float)N[k].p.z;
	}

	for (size_t i = 0; i < 6 * number_of_planes(); i += 6)
	{
		k = i / 6;
		pos centroid = (N[P[k].a].p + N[P[k].b].p + N[P[k].c].p) * 0.3333;
		pos normal_ext = centroid + P[k].normal * 0.1;

		normalpos[i + 0] = (float)(centroid.x);
		normalpos[i + 1] = (float)(centroid.y);
		normalpos[i + 2] = (float)(centroid.z);
		normals[int((i + 2) / 3)] = int((i + 2) / 3);

		normalpos[i + 3] = (float)(normal_ext.x);
		normalpos[i + 4] = (float)(normal_ext.y);
		normalpos[i + 5] = (float)(normal_ext.z);
		normals[int((i + 5) / 3)] = int((i + 5) / 3);
	}

	for (size_t i = 0; i < number_of_nodes(); ++i)
		node_index[i] = GLuint(i);

	for (size_t i = 0; i < 3 * number_of_planes(); i += 3)
	{
		k = i / 3;
		plane_index[i + 0] = GLuint(P[k].a);
		plane_index[i + 1] = GLuint(P[k].b);
		plane_index[i + 2] = GLuint(P[k].c);
	}

	for (size_t i = 0; i < 6 * number_of_planes(); i += 6)
	{
		k = i / 6;
		plane_edge_index[i + 0] = GLuint(P[k].a);
		plane_edge_index[i + 1] = GLuint(P[k].b);

		plane_edge_index[i + 2] = GLuint(P[k].a);
		plane_edge_index[i + 3] = GLuint(P[k].c);

		plane_edge_index[i + 4] = GLuint(P[k].b);
		plane_edge_index[i + 5] = GLuint(P[k].c);
	}

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

	shadersource src = parseshader("src/shaders/3d_display.glsl");
	unsigned int shader = createshader(src.vertex, src.fragment);
	glUseProgram(shader);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glClear(GL_COLOR_BUFFER_BIT);
	va_node.draw();
	va_plane.draw();
	va_plane_edge.draw();
	va_normals.draw();

	glfwSwapBuffers(window);

	glDeleteProgram(shader);
}
*/
}