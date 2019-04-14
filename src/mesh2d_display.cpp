#include "mesh2d.h"
#include "gl_abstraction.h"
/*
This file contains the definitions of display functions used class mesh
*/

using namespace std;

namespace __2d__
{

void mesh::display()
{
	window w(800, 800);
	vector<float> posdata = export_vertex_data();
	vector<GLuint> node_index = export_node_index();
	vector<GLuint> edge_index = export_edge_index();
	vector<color> node_color(number_of_nodes());
	vector<color> edge_color(number_of_nodes(), colors("red"));
	vector<float> node_size(number_of_nodes());

	pos p;
	float zoom;

	for (size_t i = 0; i < number_of_nodes(); ++i)
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

	data_buffer db_node_size(GL_ARRAY_BUFFER, node_size);
	db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_edge(GL_ELEMENT_ARRAY_BUFFER, edge_index);

	vertex_array va_node(GL_POINTS), va_edge(GL_LINES);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &db_node_size, &ib_node});
	va_node.unbind();

	va_edge.bind();
	va_edge.bind_buffer({&vb_pos, &cb_edge, &ib_edge});
	va_edge.unbind();

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

		va_node.draw();
		va_edge.draw();

		p = w.get_pos();
		zoom = w.get_zoom();
		trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
		scale = glm::scale(trans, glm::vec3(zoom));
		mvp = scale * trans;

		prog.update_uniforms();

		/*
		ImGui::Begin("Stats", (bool *)false, mf);
		ImGui::Text(stats().c_str());
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		*/

		w.swap_buffers();
		w.poll_events();
	}
}


void mesh::display_internal(window &w)
{
	vector<float> posdata = export_vertex_data();
	vector<GLuint> node_index = export_node_index();
	vector<GLuint> edge_index = export_edge_index();
	vector<color> node_color(number_of_nodes());
	vector<color> edge_color(number_of_nodes(), colors("red"));
	vector<float> node_size(number_of_nodes());

	pos p;
	float zoom;

	for (size_t i = 0; i < number_of_nodes(); ++i)
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

	data_buffer db_node_size(GL_ARRAY_BUFFER, node_size);
	db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_edge(GL_ELEMENT_ARRAY_BUFFER, edge_index);

	vertex_array va_node(GL_POINTS), va_edge(GL_LINES);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &db_node_size, &ib_node});
	va_node.unbind();

	va_edge.bind();
	va_edge.bind_buffer({&vb_pos, &cb_edge, &ib_edge});
	va_edge.unbind();

	program prog("src/shaders/display_2d.glsl");

	p = w.get_pos();
	zoom = w.get_zoom();
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
	glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
	glm::mat4 mvp = scale * trans;

	prog.set_uniform("MVP", uniform_types::MAT4F, (void *)&mvp[0][0]);
	prog.set_uniform("psize", uniform_types::FLOAT, (void *)&zoom);

	va_node.draw();
	va_edge.draw();

	p = w.get_pos();
	zoom = w.get_zoom();
	trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
	scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
	mvp = scale * trans;

	prog.update_uniforms();
}


void mesh::generate_interactive()
{
	window w(800, 800);
	bool need_to_update = true;
	ImGuiIO &io = ImGui::GetIO();
	
	unordered_map<string,void(mesh::*)(void)> command_map;
	command_map.insert(make_pair("1",generate_mesh_basic));
	command_map.insert(make_pair("2",node_insertion));
	command_map.insert(make_pair("3",refine_triangles));
	command_map.insert(make_pair("4",edge_swap));
	command_map.insert(make_pair("5",refine_triangles_near_bound));
	command_map.insert(make_pair("6",refine_triangles_near_hole));
	command_map.insert(make_pair("7",centroid_shift));
	command_map.insert(make_pair("8",generate_mesh_full));

	pos p;
	float zoom;
	vector<float> posdata;
	vector<GLuint> node_index;
	vector<GLuint> edge_index;
	vector<color> node_color;
	vector<color> edge_color;
	vector<float> node_size;

	data_buffer vb_pos(GL_ARRAY_BUFFER);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_node(GL_ARRAY_BUFFER);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_edge(GL_ARRAY_BUFFER);
	cb_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer db_node_size(GL_ARRAY_BUFFER);
	db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER), ib_edge(GL_ELEMENT_ARRAY_BUFFER);

	vertex_array va_node(GL_POINTS), va_edge(GL_LINES);

	program prog("src/shaders/display_2d.glsl");

	p = w.get_pos();
	zoom = w.get_zoom();
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
	glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
	glm::mat4 mvp = scale * trans;

	prog.set_uniform("MVP", uniform_types::MAT4F, (void *)&mvp[0][0]);
	prog.set_uniform("psize", uniform_types::FLOAT, (void *)&zoom);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);

	char text_input[256];
	bool err_message = false;
	while (!w.should_close())
	{
		if (need_to_update)
		{
			posdata = export_vertex_data();
			node_index = export_node_index();
			edge_index = export_edge_index();
			node_color = vector<color>(number_of_nodes());
			edge_color = vector<color>(number_of_nodes(), colors("red"));
			node_size = vector<float>(number_of_nodes());

			for (size_t i = 0; i < number_of_nodes(); ++i)
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

			vb_pos.data(posdata);
			cb_node.data(node_color);
			cb_edge.data(edge_color);
			db_node_size.data(node_size);
			ib_node.data(node_index);
			ib_edge.data(edge_index);

			va_node.bind();
			va_node.bind_buffer({&vb_pos, &cb_node, &db_node_size, &ib_node});
			va_node.unbind();

			va_edge.bind();
			va_edge.bind_buffer({&vb_pos, &cb_edge, &ib_edge});
			va_edge.unbind();

			need_to_update = false;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClear(GL_COLOR_BUFFER_BIT);

		va_node.draw();
		va_edge.draw();

		p = w.get_pos();
		zoom = w.get_zoom();
		trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
		scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
		mvp = scale * trans;

		prog.update_uniforms();

		ImGui::Begin("Commands");
		ImGui::Text("1:generate triangles\n"
					"2:node insertion\n"
					"3:equalize triangles\n"
					"4:edge swap\n"
			  		"5:equalize triangles near boundary\n"
					"6:equalize triangles near hole\n"
					"7:node shift\n"
					"8:auto generate\n"
					/*"9:generate ghost triangles\n"*/);
		//ImGui::SetWindowPos(ImVec2(0, 0));
		
		if (ImGui::InputText("Input", text_input, IM_ARRAYSIZE(text_input), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			string s = text_input;
			
			if (command_map.find(s)!=command_map.end())
			{
				(this->*command_map[s])();
				need_to_update = true;
				err_message = false;
			}

			else
				err_message=true;				
		}
		if(err_message)
			ImGui::Text("Command not available\n");
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		w.swap_buffers();
		w.poll_events();
	}
}

void mesh::display_node(const vector<node> &m_N)
{

	vector<GLuint> node_index(m_N.size());
	vector<color> node_color;
	vector<float> node_size;
	vector<float> posdata = export_vertex_data();
	uint64_t k = 0;
	
	for (size_t i = 0; i < m_N.size(); ++i)
	{
		node_index[i] = GLuint(m_N[i].id);
	}
	for (const node &n : N)
	{
		node_color.push_back(colors("yellow"));
		node_size.push_back(10.0);
	}
	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
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

void mesh::display_triangle(const vector<triangle> &m_T)
{
	vector<GLuint> triangle_index(6 * m_T.size());
	vector<float> posdata = export_vertex_data();
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
	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_triangle_edge(GL_ARRAY_BUFFER, edge_color);
	cb_triangle_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer ib_triangle_edge(GL_ELEMENT_ARRAY_BUFFER, triangle_index);

	vertex_array va_triangle_edge(GL_LINES);

	va_triangle_edge.bind();
	va_triangle_edge.bind_buffer({&vb_pos, &cb_triangle_edge, &ib_triangle_edge});
	va_triangle_edge.unbind();
	glLineWidth(3.0);
	va_triangle_edge.draw();
	glLineWidth(1.0);
}

void mesh::display_edge(const vector<edge> &m_E)
{
	vector<GLuint> edge_index(2 * m_E.size());
	vector<float> posdata = export_vertex_data();
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
	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_edge(GL_ARRAY_BUFFER, edge_color);
	cb_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer ib_edge(GL_ELEMENT_ARRAY_BUFFER, edge_index);

	vertex_array va_edge(GL_LINES);

	va_edge.bind();
	va_edge.bind_buffer({&vb_pos, &cb_edge, &ib_edge});
	va_edge.unbind();
	glLineWidth(3.0);
	va_edge.draw();
	glLineWidth(1.0);
}
/*
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