#include "3d.h"
#include "stl_reader.h"
using namespace std;

void _3D_::stl_read(const string filepath)
{
	ifstream f;
	vector<plane> _P;
	read_stl(f, "tests/sphere1.stl", _P);

	unordered_map<pos, uint64_t> s;
	uint64_t n = 0;
	for (const plane &p : _P)
	{
		if (s.find(p.a) == s.end())
			s.insert({p.a, n++});

		if (s.find(p.b) == s.end())
			s.insert({p.b, n++});

		if (s.find(p.c) == s.end())
			s.insert({p.c, n++});
	}

	for (const auto &[m_pos, m_id] : s)
	{
		N.push_back({m_pos, m_id, node_location::boundary});
	}

	for (const plane &p : _P)
	{
		P.push_back({s[p.a], s[p.b], s[p.c], p.normal, P.size()});
	}
}

void _3D_::display()
{
	vector<float> posdata(3 * number_of_nodes());
	vector<GLuint> node_index(number_of_nodes());
	vector<color> node_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_index(3 * number_of_planes());
	vector<color> plane_color(3 * number_of_planes(), colors("yellow"));

	uint64_t k = 0;

	for (size_t i = 0; i < 3 * number_of_nodes(); i += 3)
	{
		k = i / 3;
		posdata[i + 0] = (float)N[k].p.x;
		posdata[i + 1] = (float)N[k].p.y;
		posdata[i + 2] = (float)N[k].p.z;
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

	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_plane(GL_ELEMENT_ARRAY_BUFFER, plane_index);

	data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_plane(GL_ARRAY_BUFFER, plane_color);
	cb_plane.configure_layout(2, 3, 3, GL_FLOAT);

	vertex_array va_node(GL_POINTS), va_plane(GL_TRIANGLES);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &ib_node});
	va_node.unbind();

	va_plane.bind();
	va_plane.bind_buffer({&vb_pos, &cb_plane, &ib_plane});
	va_plane.unbind();

	shadersource src = parseshader("src/shaders/3d_display.glsl");
	unsigned int shader = createshader(src.vertex, src.fragment);
	glUseProgram(shader);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glClear(GL_COLOR_BUFFER_BIT);
	va_node.draw();
	va_plane.draw();
	glfwSwapBuffers(window);

	glDeleteProgram(shader);
}