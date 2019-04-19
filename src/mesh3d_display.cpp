#include "mesh3d.h"
#include "gl_abstraction.h"

using namespace std;
namespace __3d__
{

void mesh::display()
{

	window w(800, 800);

	vector<float> posdata = export_vertex_data();
	vector<GLuint> node_index = export_node_index();
	vector<color> node_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_index = export_plane_index();
	vector<color> plane_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_edge_index = export_plane_edge_index();
	vector<color> plane_edge_color(number_of_nodes(), colors("red"));
	auto [normalpos,normals] = export_normals();
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

void mesh::display_internal(window &w, bool draw_normals)
{
	vector<float> posdata = export_vertex_data();
	vector<GLuint> node_index = export_node_index();
	vector<color> node_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_index = export_plane_index();
	vector<color> plane_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> plane_edge_index = export_plane_edge_index();
	vector<color> plane_edge_color(number_of_nodes(), colors("red"));
	auto [normalpos,normals] = export_normals();
	vector<color> normal_color(2 * number_of_planes(), colors("blue"));

	//pos p;
	//float rotx, roty;
	

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

	/*
	p = w.get_pos();
	rotx = w.get_rotx();
	roty = w.get_roty();
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

	program prog("src/shaders/3d_ind.glsl");
	prog.set_uniform("MVP", uniform_types::MAT4F, &View[0][0]);
*/
	va_node.draw();
	va_plane.draw();
	va_plane_edge.draw();

	if (draw_normals)
			va_normals.draw();
	/*
	p = w.get_pos();
	rotx = w.get_rotx();
	roty = w.get_roty();
	ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
	ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

	prog.update_uniforms();
	*/
}

void mesh::display_nodes(const vector<node> &m_N)
{
	vector<GLuint> node_index(m_N.size());
	vector<color> node_color;
	vector<float> posdata = export_vertex_data();
	uint64_t k = 0;
	
	for (size_t i = 0; i < m_N.size(); ++i)
	{
		node_index[i] = GLuint(m_N[i].id);
	}
	for (const node &n : N)
	{
		node_color.push_back(colors("turquoise"));
	}
	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index);

	vertex_array va_node(GL_POINTS);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &ib_node});
	va_node.unbind();

	va_node.draw();
}

void mesh::display_planes(const vector<plane> &m_P,bool draw_normals, color m_color)
{
	vector<GLuint> plane_index(3 * m_P.size());
	vector<float> posdata = export_vertex_data();
	vector<color> plane_color;
	vector<float> npos(6*m_P.size());
	vector<uint32_t> normal_index(2*m_P.size());
	vector<color> normal_color(2*m_P.size(),colors("purple"));

	uint64_t k;
	for (size_t i = 0; i < 3 * m_P.size(); i += 3)
	{
		k = i / 3;
		plane_index[i + 0] = GLuint(m_P[k].a);
		plane_index[i + 1] = GLuint(m_P[k].b);
		plane_index[i + 2] = GLuint(m_P[k].c);
	
	}

	for (size_t i = 0; i < 6 * m_P.size(); i += 6)
	{
		k = i / 6;
		pos centroid = (N[P[k].a].p + N[P[k].b].p + N[P[k].c].p) * 0.3333;
		pos normal_ext = centroid + P[k].normal * 0.1;

		npos[i + 0] = (float)(centroid.x);
		npos[i + 1] = (float)(centroid.y);
		npos[i + 2] = (float)(centroid.z);
		normal_index[int((i + 2) / 3)] = uint32_t((i + 2) / 3);

		npos[i + 3] = (float)(normal_ext.x);
		npos[i + 4] = (float)(normal_ext.y);
		npos[i + 5] = (float)(normal_ext.z);
		normal_index[int((i + 5) / 3)] = uint32_t((i + 5) / 3);
	}

	for (const node &n : N)
		plane_color.push_back(m_color);

	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer vb_npos(GL_ARRAY_BUFFER,npos);
	vb_npos.configure_layout(1, 3, 3, GL_FLOAT);

	data_buffer cb_triangle_edge(GL_ARRAY_BUFFER, plane_color);
	cb_triangle_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_normal(GL_ARRAY_BUFFER, normal_color);
	cb_normal.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer ib_plane(GL_ELEMENT_ARRAY_BUFFER, plane_index),ib_normal(GL_ELEMENT_ARRAY_BUFFER,normal_index);

	vertex_array va_plane(GL_TRIANGLES),va_normals(GL_LINES);

	va_plane.bind();
	va_plane.bind_buffer({&vb_pos, &cb_triangle_edge, &ib_plane});
	va_plane.unbind();

	va_normals.bind();
	va_normals.bind_buffer({&vb_npos, &cb_normal, &ib_normal});
	va_normals.unbind();

	va_plane.draw();
	if(draw_normals)
		va_normals.draw();
}
/*
void mesh::display3d()
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
	vector<color> node_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> tetrahedron_index(12 * number_of_tetrahedrons());
	vector<color> tetrahedron_color(number_of_nodes(), colors("yellow"));
	vector<GLuint> tetrahedron_edge_index(24 * number_of_tetrahedrons());
	vector<color> tetrahedron_edge_color(number_of_nodes(), colors("red"));
	//vector<float> normalpos(6 * number_of_planes());
	//vector<GLuint> normals(2*number_of_planes());
	//vector<color> normal_color(2*number_of_planes(),colors("blue"));

	uint64_t k = 0;

	for (size_t i = 0; i < 3 * number_of_nodes(); i += 3)
	{
		k = i / 3;
		posdata[i + 0] = (float)N[k].p.x;
		posdata[i + 1] = (float)N[k].p.y;
		posdata[i + 2] = (float)N[k].p.z;
	}
	/*
	for (size_t i = 0; i < 6 * number_of_planes(); i += 6)
	{
		k = i / 6;
		pos centroid = (N[P[k].a].p + N[P[k].b].p + N[P[k].c].p)*0.3333;
		pos normal_ext = centroid + P[k].normal*0.1;

		normalpos[i + 0] = (float)(centroid.x);
		normalpos[i + 1] = (float)(centroid.y);
		normalpos[i + 2] = (float)(centroid.z);
		normals[int((i+2)/3)] = int((i+2)/3);

		normalpos[i + 3] = (float)(normal_ext.x);
		normalpos[i + 4] = (float)(normal_ext.y);
		normalpos[i + 5] = (float)(normal_ext.z);
		normals[int((i+5)/3)] = int((i+5)/3);
	}
	*/
/*
	for (size_t i = 0; i < number_of_nodes(); ++i)
		node_index[i] = GLuint(i);

	for (size_t i = 0; i < 12 * number_of_tetrahedrons(); i += 12)
	{
		k = i / 12;
		tetrahedron_index[i + 0] = GLuint(TH[k].a);
		tetrahedron_index[i + 1] = GLuint(TH[k].b);
		tetrahedron_index[i + 2] = GLuint(TH[k].c);

		tetrahedron_index[i + 3] = GLuint(TH[k].a);
		tetrahedron_index[i + 4] = GLuint(TH[k].b);
		tetrahedron_index[i + 5] = GLuint(TH[k].d);

		tetrahedron_index[i + 6] = GLuint(TH[k].a);
		tetrahedron_index[i + 7] = GLuint(TH[k].d);
		tetrahedron_index[i + 8] = GLuint(TH[k].c);

		tetrahedron_index[i + 9] = GLuint(TH[k].d);
		tetrahedron_index[i + 10] = GLuint(TH[k].b);
		tetrahedron_index[i + 11] = GLuint(TH[k].c);
	}

	for (size_t i = 0; i < 24 * number_of_tetrahedrons(); i += 24)
	{
		k = i / 24;
		tetrahedron_edge_index[i + 0] = GLuint(TH[k].a);
		tetrahedron_edge_index[i + 1] = GLuint(TH[k].b);

		tetrahedron_edge_index[i + 2] = GLuint(TH[k].a);
		tetrahedron_edge_index[i + 3] = GLuint(TH[k].c);

		tetrahedron_edge_index[i + 4] = GLuint(TH[k].b);
		tetrahedron_edge_index[i + 5] = GLuint(TH[k].c);

		tetrahedron_edge_index[i + 6] = GLuint(TH[k].a);
		tetrahedron_edge_index[i + 7] = GLuint(TH[k].b);

		tetrahedron_edge_index[i + 8] = GLuint(TH[k].a);
		tetrahedron_edge_index[i + 9] = GLuint(TH[k].d);

		tetrahedron_edge_index[i + 10] = GLuint(TH[k].b);
		tetrahedron_edge_index[i + 11] = GLuint(TH[k].d);

		tetrahedron_edge_index[i + 12] = GLuint(TH[k].a);
		tetrahedron_edge_index[i + 13] = GLuint(TH[k].d);

		tetrahedron_edge_index[i + 14] = GLuint(TH[k].a);
		tetrahedron_edge_index[i + 15] = GLuint(TH[k].c);

		tetrahedron_edge_index[i + 16] = GLuint(TH[k].d);
		tetrahedron_edge_index[i + 17] = GLuint(TH[k].c);

		tetrahedron_edge_index[i + 18] = GLuint(TH[k].d);
		tetrahedron_edge_index[i + 19] = GLuint(TH[k].b);

		tetrahedron_edge_index[i + 20] = GLuint(TH[k].d);
		tetrahedron_edge_index[i + 21] = GLuint(TH[k].c);

		tetrahedron_edge_index[i + 22] = GLuint(TH[k].b);
		tetrahedron_edge_index[i + 23] = GLuint(TH[k].c);
	}

	data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
	vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

	
	//data_buffer vb_normal(GL_ARRAY_BUFFER, normalpos);
	//vb_normal.configure_layout(1, 3, 3, GL_FLOAT);
	

	data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_tetrahedron(GL_ELEMENT_ARRAY_BUFFER, tetrahedron_index),
		ib_tetrahedron_edge(GL_ELEMENT_ARRAY_BUFFER, tetrahedron_edge_index);

	data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
	cb_node.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_tetrahedron_edge(GL_ARRAY_BUFFER, tetrahedron_edge_color);
	cb_tetrahedron_edge.configure_layout(2, 3, 3, GL_FLOAT);

	data_buffer cb_tetrahedron(GL_ARRAY_BUFFER, tetrahedron_color);
	cb_tetrahedron.configure_layout(2, 3, 3, GL_FLOAT);

	//data_buffer cb_normal(GL_ARRAY_BUFFER, normal_color);
	//cb_normal.configure_layout(2, 3, 3, GL_FLOAT);

	//data_buffer ib_normal(GL_ELEMENT_ARRAY_BUFFER,normals);

	vertex_array va_node(GL_POINTS), va_tetrahedron(GL_TRIANGLES), va_tetrahedron_edge(GL_LINES); //va_normals(GL_LINES);

	va_node.bind();
	va_node.bind_buffer({&vb_pos, &cb_node, &ib_node});
	va_node.unbind();

	va_tetrahedron.bind();
	va_tetrahedron.bind_buffer({&vb_pos, &cb_tetrahedron, &ib_tetrahedron});
	va_tetrahedron.unbind();

	va_tetrahedron_edge.bind();
	va_tetrahedron_edge.bind_buffer({&vb_pos, &cb_tetrahedron_edge, &ib_tetrahedron_edge});
	va_tetrahedron_edge.unbind();

	
	//va_normals.bind();
	//va_normals.bind_buffer({&vb_normal,&cb_normal,&ib_normal});
	//va_normals.unbind();
	

	shadersource src = parseshader("src/shaders/3d_ind.glsl");
	unsigned int shader = createshader(src.vertex, src.fragment);
	glUseProgram(shader);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glm::mat4 ViewTranslate = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 ViewRotateX = glm::rotate(
		ViewTranslate,
		rotx,
		glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 View = glm::rotate(
		ViewRotateX,
		roty,
		glm::vec3(0.0f, 1.0f, 0.0f));

	uniform MVP(shader, "MVP", uniform_types::MAT4F, (void *)&View[0][0]);
	glEnable(GL_DEPTH_TEST);
	while (!return_to_console)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		va_node.draw();
		va_tetrahedron.draw();
		va_tetrahedron_edge.draw();
		//va_normals.draw();

		ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
		View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

		MVP.update();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shader);
}

void mesh::node_share_dsiplay()
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
	vector<color> node_color(number_of_nodes(), colors("yellow"));

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

	for (const node &n : N)
	{
		vector<GLuint> tetrahedron_index(12 * n.TH.size());
		vector<color> tetrahedron_color(number_of_nodes(), colors("yellow"));
		vector<GLuint> tetrahedron_edge_index(24 * n.TH.size());
		vector<color> tetrahedron_edge_color(number_of_nodes(), colors("red"));

		for (size_t i = 0; i < 12 * n.TH.size(); i += 12)
		{
			k = i / 12;
			tetrahedron_index[i + 0] = GLuint(TH[n.TH[k]].a);
			tetrahedron_index[i + 1] = GLuint(TH[n.TH[k]].b);
			tetrahedron_index[i + 2] = GLuint(TH[n.TH[k]].c);

			tetrahedron_index[i + 3] = GLuint(TH[n.TH[k]].a);
			tetrahedron_index[i + 4] = GLuint(TH[n.TH[k]].b);
			tetrahedron_index[i + 5] = GLuint(TH[n.TH[k]].d);

			tetrahedron_index[i + 6] = GLuint(TH[n.TH[k]].a);
			tetrahedron_index[i + 7] = GLuint(TH[n.TH[k]].d);
			tetrahedron_index[i + 8] = GLuint(TH[n.TH[k]].c);

			tetrahedron_index[i + 9] = GLuint(TH[n.TH[k]].d);
			tetrahedron_index[i + 10] = GLuint(TH[n.TH[k]].b);
			tetrahedron_index[i + 11] = GLuint(TH[n.TH[k]].c);
		}

		for (size_t i = 0; i < 24 * n.TH.size(); i += 24)
		{
			k = i / 24;
			tetrahedron_edge_index[i + 0] = GLuint(TH[n.TH[k]].a);
			tetrahedron_edge_index[i + 1] = GLuint(TH[n.TH[k]].b);

			tetrahedron_edge_index[i + 2] = GLuint(TH[n.TH[k]].a);
			tetrahedron_edge_index[i + 3] = GLuint(TH[n.TH[k]].c);

			tetrahedron_edge_index[i + 4] = GLuint(TH[n.TH[k]].b);
			tetrahedron_edge_index[i + 5] = GLuint(TH[n.TH[k]].c);

			tetrahedron_edge_index[i + 6] = GLuint(TH[n.TH[k]].a);
			tetrahedron_edge_index[i + 7] = GLuint(TH[n.TH[k]].b);

			tetrahedron_edge_index[i + 8] = GLuint(TH[n.TH[k]].a);
			tetrahedron_edge_index[i + 9] = GLuint(TH[n.TH[k]].d);

			tetrahedron_edge_index[i + 10] = GLuint(TH[n.TH[k]].b);
			tetrahedron_edge_index[i + 11] = GLuint(TH[n.TH[k]].d);

			tetrahedron_edge_index[i + 12] = GLuint(TH[n.TH[k]].a);
			tetrahedron_edge_index[i + 13] = GLuint(TH[n.TH[k]].d);

			tetrahedron_edge_index[i + 14] = GLuint(TH[n.TH[k]].a);
			tetrahedron_edge_index[i + 15] = GLuint(TH[n.TH[k]].c);

			tetrahedron_edge_index[i + 16] = GLuint(TH[n.TH[k]].d);
			tetrahedron_edge_index[i + 17] = GLuint(TH[n.TH[k]].c);

			tetrahedron_edge_index[i + 18] = GLuint(TH[n.TH[k]].d);
			tetrahedron_edge_index[i + 19] = GLuint(TH[n.TH[k]].b);

			tetrahedron_edge_index[i + 20] = GLuint(TH[n.TH[k]].d);
			tetrahedron_edge_index[i + 21] = GLuint(TH[n.TH[k]].c);

			tetrahedron_edge_index[i + 22] = GLuint(TH[n.TH[k]].b);
			tetrahedron_edge_index[i + 23] = GLuint(TH[n.TH[k]].c);
		}

		data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
		vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

		data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_tetrahedron(GL_ELEMENT_ARRAY_BUFFER, tetrahedron_index),
			ib_tetrahedron_edge(GL_ELEMENT_ARRAY_BUFFER, tetrahedron_edge_index);

		data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
		cb_node.configure_layout(2, 3, 3, GL_FLOAT);

		data_buffer cb_tetrahedron_edge(GL_ARRAY_BUFFER, tetrahedron_edge_color);
		cb_tetrahedron_edge.configure_layout(2, 3, 3, GL_FLOAT);

		data_buffer cb_tetrahedron(GL_ARRAY_BUFFER, tetrahedron_color);
		cb_tetrahedron.configure_layout(2, 3, 3, GL_FLOAT);

		vertex_array va_node(GL_POINTS), va_tetrahedron(GL_TRIANGLES), va_tetrahedron_edge(GL_LINES);

		va_node.bind();
		va_node.bind_buffer({&vb_pos, &cb_node, &ib_node});
		va_node.unbind();

		va_tetrahedron.bind();
		va_tetrahedron.bind_buffer({&vb_pos, &cb_tetrahedron, &ib_tetrahedron});
		va_tetrahedron.unbind();

		va_tetrahedron_edge.bind();
		va_tetrahedron_edge.bind_buffer({&vb_pos, &cb_tetrahedron_edge, &ib_tetrahedron_edge});
		va_tetrahedron_edge.unbind();

		vector<GLuint> snode_id = {GLuint(n.id)};
		vector<color> snode_color = {colors("green")};
		data_buffer cb_sn(GL_ARRAY_BUFFER, snode_color), ib_sn(GL_ELEMENT_ARRAY_BUFFER, snode_id);
		vertex_array va_sn(GL_POINTS);
		va_sn.bind();
		va_sn.bind_buffer({&vb_pos, &cb_sn, &ib_sn});

		shadersource src = parseshader("src/shaders/3d_ind.glsl");
		unsigned int shader = createshader(src.vertex, src.fragment);
		glUseProgram(shader);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_DEPTH_TEST);
		glm::mat4 ViewTranslate = glm::translate(
			glm::mat4(1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 ViewRotateX = glm::rotate(
			ViewTranslate,
			rotx,
			glm::vec3(-1.0f, 0.0f, 0.0f));
		glm::mat4 View = glm::rotate(
			ViewRotateX,
			roty,
			glm::vec3(0.0f, 1.0f, 0.0f));

		uniform MVP(shader, "MVP", uniform_types::MAT4F, (void *)&View[0][0]);
		cout << n.TH.size() << endl;
		while (!return_to_console)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			va_node.draw();
			va_tetrahedron.draw();
			va_tetrahedron_edge.draw();
			va_sn.draw();

			ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
			View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

			MVP.update();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		return_to_console = false;
		glDeleteProgram(shader);
	}
}
*/
} // namespace __3d__