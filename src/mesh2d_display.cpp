#include "mesh2d.h"

/*
This file contains the definitions of display functions used class mesh
*/

using namespace std;

bool return_to_console;
bool cursor_inside;
bool find_value;

pos p;
double dx;
double dy;
double dz;
float zoom;
float zoom_change;
float rotx, roty, rotch;

namespace __2d__
{
//updated on 23/8/18
void mesh::display(window *w, bool inspect)
{
	p = {0, 0, 0};
	dx = 0.05;
	dy = 0.05;
	dz = 0.05;
	zoom = 1.0;
	zoom_change = 1.1;
	return_to_console = false;
	rotx = 0, roty = 0, rotch = 0.1;

	vector<float> posdata(3 * number_of_nodes());
	vector<GLuint> node_index(number_of_nodes());
	vector<GLuint> triangle_index(6 * number_of_triangles());
	vector<color> node_color(number_of_nodes());
	vector<color> triangle_edge_color(number_of_nodes());
	vector<float> node_size(number_of_nodes());

	uint64_t k = 0;

	for (size_t i = 0; i < 3 * number_of_nodes(); i += 3)
	{
		k = i / 3;
		posdata[i + 0] = (float)N[k].p.x;
		posdata[i + 1] = (float)N[k].p.y;
		posdata[i + 2] = (float)N[k].p.z;
	}

	for (size_t i = 0; i < number_of_nodes(); ++i)
	{
		node_index[i] = GLuint(i);
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

	for (size_t i = 0; i < 6 * number_of_triangles(); i += 6)
	{
		k = i / 6;
		triangle_index[i + 0] = GLuint(T[k].a);
		triangle_index[i + 1] = GLuint(T[k].b);

		triangle_index[i + 2] = GLuint(T[k].b);
		triangle_index[i + 3] = GLuint(T[k].c);

		triangle_index[i + 4] = GLuint(T[k].c);
		triangle_index[i + 5] = GLuint(T[k].a);

		if (T[k].type == triangle_type::domain)
		{
			triangle_edge_color[T[k].a] = colors("red");
			triangle_edge_color[T[k].b] = colors("red");
			triangle_edge_color[T[k].c] = colors("red");
		}

		else
		{
			triangle_edge_color[T[k].a] = colors("turquoise");
			triangle_edge_color[T[k].b] = colors("turquoise");
			triangle_edge_color[T[k].c] = colors("turquoise");
		}
	}

	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_triangle_edge(GL_ARRAY_BUFFER, triangle_edge_color);
	cb_triangle_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer db_node_size(GL_ARRAY_BUFFER, node_size);
	db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_triangle_edge(GL_ELEMENT_ARRAY_BUFFER, triangle_index);

	vertex_array va_node(GL_POINTS), va_triangle_edge(GL_LINES);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &db_node_size, &ib_node});
	va_node.unbind();

	va_triangle_edge.bind();
	va_triangle_edge.bind_buffer({&vb_pos, &cb_triangle_edge, &ib_triangle_edge});
	va_triangle_edge.unbind();

	program prog("src/shaders/display_2d.glsl");

	glEnable(GL_PROGRAM_POINT_SIZE);

	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
	glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
	glm::mat4 mvp = trans * scale;

	uniform MVP(prog.get_id(), "MVP", uniform_types::MAT4F, (void *)&mvp[0][0]);
	uniform psize(prog.get_id(), "psize", uniform_types::FLOAT, (void *)&zoom);

	if (!inspect)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		va_node.draw();
		va_triangle_edge.draw();

		trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
		scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
		mvp = trans * scale;

		MVP.update();
		psize.update();

		w->swap_buffers();
	}

	else // if (inspect)
	{
		while (!return_to_console)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			va_node.draw();
			va_triangle_edge.draw();

			trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
			scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
			mvp = trans * scale;

			MVP.update();
			psize.update();

			w->swap_buffers();
			w->poll_events();
		}
	}
}

/*
void mesh::display_node(const vector<node> &m_N)
{

	vector<GLuint> node_index(m_N.size());
	vector<color> node_color;
	vector<float> node_size;
	//vector<float> posdata(3*m_N.size());
	uint64_t k = 0;
	/*
	for (size_t i = 0; i < 3* m_N.size(); i+=3)
		{
			k = i/3;
			posdata[i+0] = (float)m_N[k].p.x;
			posdata[i+1] = (float)m_N[k].p.y;
			posdata[i+2] = (float)m_N[k].p.z;
		}
*/
/*
	for (size_t i = 0; i < m_N.size(); ++i)
	{
		node_index[i] = GLuint(m_N[i].id);
	}
	for (const node &n : N)
	{
		node_color.push_back(colors("yellow"));
		node_size.push_back(10.0);
	}
	data_buffer vb_pos(GL_ARRAY_BUFFER, pdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer db_node_size(GL_ARRAY_BUFFER, node_size);
	db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index);

	vertex_array va_node(GL_POINTS);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &db_node_size, &ib_node});
	va_node.unbind();

	va_node.draw();
}

void mesh::display_triangle(const vector<mesh_triangle> &m_T)
{
	vector<GLuint> triangle_index(6 * m_T.size());

	vector<color> edge_color;
	uint64_t k;
	for (size_t i = 0; i < 6 * m_T.size(); i += 6)
	{
		k = i / 6;
		triangle_index[i + 0] = GLuint(m_T[k].a);
		triangle_index[i + 1] = GLuint(m_T[k].b);

		triangle_index[i + 2] = GLuint(m_T[k].b);
		triangle_index[i + 3] = GLuint(m_T[k].c);

		triangle_index[i + 4] = GLuint(m_T[k].c);
		triangle_index[i + 5] = GLuint(m_T[k].a);
	}
	for (const node &n : N)
		edge_color.push_back(colors("yellow"));
	data_buffer vb_pos(GL_ARRAY_BUFFER, pdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_triangle_edge(GL_ARRAY_BUFFER, edge_color);
	cb_triangle_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer ib_triangle_edge(GL_ELEMENT_ARRAY_BUFFER, triangle_index);

	vertex_array va_triangle_edge(GL_LINES);

	va_triangle_edge.bind();
	va_triangle_edge.bind_buffer({&vb_pos, &cb_triangle_edge, &ib_triangle_edge});
	va_triangle_edge.unbind();

	va_triangle_edge.draw();
}

void mesh::display_edge(const vector<edge> &m_E)
{
	vector<GLuint> edge_index(2 * m_E.size());

	vector<color> edge_color;
	uint64_t k;
	for (size_t i = 0; i < 2 * m_E.size(); i += 2)
	{
		k = i / 2;
		edge_index[i + 0] = GLuint(m_E[k].start);
		edge_index[i + 1] = GLuint(m_E[k].end);
	}
	for (const node &n : N)
		edge_color.push_back(colors("yellow"));
	data_buffer vb_pos(GL_ARRAY_BUFFER, pdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_edge(GL_ARRAY_BUFFER, edge_color);
	cb_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer ib_edge(GL_ELEMENT_ARRAY_BUFFER, edge_index);

	vertex_array va_edge(GL_LINES);

	va_edge.bind();
	va_edge.bind_buffer({&vb_pos, &cb_edge, &ib_edge});
	va_edge.unbind();

	va_edge.draw();
}

void mesh::display_all_nodes()
{
	if (N.size() != 0)
	{
		vector<GLuint> node_index(N.size());
		vector<color> node_color(N.size());
		vector<float> node_size(N.size());
		//vector<float> posdata(3*N.size());
		uint64_t k = 0;
		/*
		for (size_t i = 0; i < 3 * number_of_nodes(); i += 3)
		{
			k = i / 3;
			posdata[i + 0] = (float)N[k].p.x;
			posdata[i + 1] = (float)N[k].p.y;
			posdata[i + 2] = (float)N[k].p.z;
		}
*/
/*
		for (size_t i = 0; i < number_of_nodes(); ++i)
		{
			node_index[i] = GLuint(i);
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

		data_buffer vb_pos(GL_ARRAY_BUFFER, pdata);
		vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

		data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
		cb_node.configure_layout(2, 3, 3, GL_FLOAT);

		data_buffer db_node_size(GL_ARRAY_BUFFER, node_size);
		db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

		data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index);

		vertex_array va_node(GL_POINTS);

		va_node.bind();
		va_node.bind_buffer({&vb_pos, &cb_node, &db_node_size, &ib_node});
		va_node.unbind();

		va_node.draw();
		//glfwSwapBuffers(window);
	}
}

void mesh::display_all_edges()
{
	if (E.size() != 0)
	{
		vector<GLuint> edge_index(2 * number_of_edges());
		vector<color> edge_color(number_of_nodes());

		uint64_t k;
		for (size_t i = 0; i < 2 * number_of_edges(); i += 2)
		{
			k = i / 2;
			edge_index[i + 0] = GLuint(E[k].start);
			edge_index[i + 1] = GLuint(E[k].end);

			edge_color[E[k].start] = colors("red");
			edge_color[E[k].end] = colors("red");
		}

		data_buffer vb_pos(GL_ARRAY_BUFFER, pdata);
		vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

		data_buffer cb_edge(GL_ARRAY_BUFFER, edge_color);
		cb_edge.configure_layout(2, 3, 3, GL_FLOAT);

		data_buffer ib_edge(GL_ELEMENT_ARRAY_BUFFER, edge_index);

		vertex_array va_edge(GL_LINES);

		va_edge.bind();
		va_edge.bind_buffer({&vb_pos, &cb_edge, &ib_edge});
		va_edge.unbind();

		va_edge.draw();
	}
}

void mesh::display_all_triangles()
{
	if (T.size() != 0)
	{
		vector<GLuint> triangle_index(6 * number_of_triangles());
		vector<color> triangle_edge_color(number_of_nodes());

		uint64_t k;
		for (size_t i = 0; i < 6 * number_of_triangles(); i += 6)
		{
			k = i / 6;
			triangle_index[i + 0] = GLuint(T[k].a);
			triangle_index[i + 1] = GLuint(T[k].b);

			triangle_index[i + 2] = GLuint(T[k].b);
			triangle_index[i + 3] = GLuint(T[k].c);

			triangle_index[i + 4] = GLuint(T[k].c);
			triangle_index[i + 5] = GLuint(T[k].a);

			if (T[k].type == triangle_type::domain)
			{
				triangle_edge_color[T[k].a] = colors("red");
				triangle_edge_color[T[k].b] = colors("red");
				triangle_edge_color[T[k].c] = colors("red");
			}

			else
			{
				triangle_edge_color[T[k].a] = colors("turquoise");
				triangle_edge_color[T[k].b] = colors("turquoise");
				triangle_edge_color[T[k].c] = colors("turquoise");
			}
		}

		data_buffer vb_pos(GL_ARRAY_BUFFER, pdata);
		vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

		data_buffer cb_triangle_edge(GL_ARRAY_BUFFER, triangle_edge_color);
		cb_triangle_edge.configure_layout(2, 3, 3, GL_FLOAT);

		data_buffer ib_triangle_edge(GL_ELEMENT_ARRAY_BUFFER, triangle_index);

		vertex_array va_triangle_edge(GL_LINES);

		va_triangle_edge.bind();
		va_triangle_edge.bind_buffer({&vb_pos, &cb_triangle_edge, &ib_triangle_edge});
		va_triangle_edge.unbind();

		va_triangle_edge.draw();
	}
}

*/

} // namespace __2d__