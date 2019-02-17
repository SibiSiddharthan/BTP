#include "mesh.h"

bool return_to_console;
bool cursor_inside;
bool find_value;

pos p;
double dx;
double dy;
double dz;
float zoom;
float zoom_change;

using namespace std;

void mesh::init_2d()
{
	type = mesh_type::_2d;
}

//updated on 29/8/18
void import_2d(mesh &M, const _2D_ &_2d_)
{
	M.init_2d();

	M.N = _2d_.N;
	M.E = _2d_.E;
}

//updated on 29/8/18
//Displays the number of nodes,number of triangles and the average area of the triangles in the mesh
void mesh::stats()
{
	cout << "Number of nodes: " << number_of_nodes() << endl;
	cout << "Number of edges: " << number_of_edges() << endl;
	cout << "Number of triangles: " << number_of_triangles() << endl;
	cout << "Average area of triangles: " << avg_area_of_triangles() << endl;
}

//updated on 23/8/18

void mesh::generate_mesh_basic(bool debug)
{
	edge e1, e2;
	int64_t e_id;
	double min_dist = 1.0e10;
	double temp;
	int k = -1;
	while (number_of_unused_edges(E) != 0)
	{
		for (size_t i = 0; i < number_of_edges(); i++)
		{
			k = -1;
			min_dist = 1.0e10;
			for (size_t j = 0; j < number_of_nodes(); j++)
			{
				if (E[i].availability && N[j].availability && (E[i].start != N[j].id && E[i].end != N[j].id))
				{
					if (left_test(E[i], N[j]) && collinear_test(E[i], N[j]))
					{

						e1.start = N[j].id;
						e1.end = E[i].end;
						e2.start = E[i].start;
						e2.end = N[j].id;

						if (intersection_test(e1, E) && intersection_test(e2, E))
						{
							temp = distance((N[E[i].start].p + N[E[i].end].p) * 0.5, N[j].p);
							if (temp < min_dist)
							{
								k = j;
								min_dist = temp;
							}
						}
					}
				}
			}
			if (debug && k > -1)
			{
				glfwPostEmptyEvent();
				cin.get();
				cin.get();
				update_pdata();
				//glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
				//glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
				//glm::mat4 mvp = trans * scale;

				shadersource src = parseshader("src/shaders/debug_display.glsl");
				unsigned int shader = createshader(src.vertex, src.fragment);
				glUseProgram(shader);

				glEnable(GL_PROGRAM_POINT_SIZE);
				glEnable(GL_LINE_SMOOTH);
				//uniform MVP(shader, "MVP", uniform_types::MAT4F, (void *)&mvp[0][0]);
				//uniform psize(shader, "psize", uniform_types::FLOAT, (void *)&zoom);
				glClear(GL_COLOR_BUFFER_BIT);
				display_all_nodes();
				display_all_edges();
				display_node({N[k], N[E[i].start], N[E[i].end]});
				glLineWidth(3.0);
				display_edge({E[i]});
				window_swap_buffers();
				//trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
				//scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
				//mvp = trans * scale;
				//MVP.update();
				//psize.update();

				glfwWaitEventsTimeout(100.0);
				cout << "Enter for next\n";
				cin.get();
			}
			if (E[i].availability && k > -1)
			{

				e1.start = N[k].id;
				e1.end = E[i].end;
				e2.start = E[i].start;
				e2.end = N[k].id;

				e_id = edge_exists(E, e1);
				if (e_id != -1)
				{
					E[e_id].availability = false;
					e1.id = e_id;
					disable_common_node(e1, E[i]);
				}

				else
					make_inside_edge(e1.start, e1.end, true);

				e_id = edge_exists(E, e2);
				if (e_id != -1)
				{
					E[e_id].availability = false;
					e2.id = e_id;
					disable_common_node(e2, E[i]);
				}
				else
					make_inside_edge(e2.start, e2.end, true);

				make_triangle(E[i].start, E[i].end, N[k].id);
				E[i].availability = false;
			}
		}
	}
}

//updated on 23/8/18
void mesh::node_insertion()
{
	for (size_t i = 0; i < number_of_nodes(); i++)
	{
		if (N[i].location == node_location::boundary || N[i].location == node_location::hole)
		{
			if (N[i].triangle_share() > 3)
			{
				unordered_set<node_id> S;
				vector<node> temp;
				pos centroid;
				pair<node_id, node_id> np;
				for (const triangle_id t_id : N[i].T)
				{
					S.insert(T[t_id].a);
					S.insert(T[t_id].b);
					S.insert(T[t_id].c);
				}

				for (const node_id n_id : S)
					temp.push_back(N[n_id]);

				centroid = generate_centroid(temp);
				np = corner_pos(N[i]);

				if (distance((N[np.first].p + N[np.second].p) * 0.5, N[i].p) < distance((N[np.first].p + N[np.second].p) * 0.5, centroid))
				{
					N.push_back({centroid, N.size(), node_location::inside, false});

					for (size_t j = 0; j < N[i].triangle_share(); j++)
						triangle_node_change(N[i].T[j], N[i].id, N[N.size() - 1].id);

					for (size_t j = 0; j < N[i].edge_share(); j++)
						edge_node_change(N[i].IE[j], N[i].id, N[N.size() - 1].id);

					N[i].T.clear();
					N[i].IE.clear();

					make_triangle(N[N.size() - 1].id, N[i].id, np.first);
					make_triangle(N[N.size() - 1].id, N[i].id, np.second);
					make_inside_edge(N[N.size() - 1].id, N[i].id, false);
					make_inside_edge(N[N.size() - 1].id, N[np.first].id, false);
					make_inside_edge(N[N.size() - 1].id, N[np.second].id, false);
				}
			}
		}
	}
}

//updated on 23/8/18
void mesh::refine_triangles()
{
	double avg = avg_area_of_triangles();

	uint64_t flag = 0;
	if (avg > area_threshold)
	{
		do
		{
			vector<mesh_triangle> temp;
			flag = 0;
			for (const mesh_triangle &m_t : T)
			{
				if (double area = triangle_area(m_t); area > 2 * avg && area > 2.5 * area_threshold)
				{
					N.push_back({(N[m_t.a].p + N[m_t.b].p + N[m_t.c].p) / 3.0, N.size(), node_location::inside, false});

					temp.push_back({N[N.size() - 1].id, m_t.b, m_t.c, temp.size()});
					temp.push_back({m_t.a, N[N.size() - 1].id, m_t.c, temp.size()});
					temp.push_back({m_t.a, m_t.b, N[N.size() - 1].id, temp.size()});

					make_inside_edge(N[N.size() - 1].id, m_t.a, false);
					make_inside_edge(N[N.size() - 1].id, m_t.b, false);
					make_inside_edge(N[N.size() - 1].id, m_t.c, false);
				}
				else
					temp.push_back({m_t.a, m_t.b, m_t.c, temp.size()});
			}

			T = temp;
			for (const mesh_triangle &m_t : T)
				if (triangle_area(m_t) > (2 * avg))
					++flag;
		} while (flag != 0);
	}
	node_triangle_share_sweep();
}

//updated on 23/8/18
void mesh::refine_triangles_near_boundary(node_location _location)
{

	double avg = avg_area_of_triangles_near_boundary(_location);

	if (avg > area_threshold)
	{
		vector<mesh_triangle> temp;
		for (const mesh_triangle &m_t : T)
		{
			if (N[m_t.a].location == _location || N[m_t.b].location == _location || N[m_t.c].location == _location)
			{
				if (double area = triangle_area(m_t); area > 1.5 * avg && area > 2 * area_threshold)
				{
					N.push_back({(N[m_t.a].p + N[m_t.b].p + N[m_t.c].p) / 3.0, N.size(), node_location::inside, false});

					temp.push_back({N[N.size() - 1].id, m_t.b, m_t.c, temp.size()});
					temp.push_back({m_t.a, N[N.size() - 1].id, m_t.c, temp.size()});
					temp.push_back({m_t.a, m_t.b, N[N.size() - 1].id, temp.size()});

					make_inside_edge(N[N.size() - 1].id, m_t.a, false);
					make_inside_edge(N[N.size() - 1].id, m_t.b, false);
					make_inside_edge(N[N.size() - 1].id, m_t.c, false);
				}
				else
					temp.push_back({m_t.a, m_t.b, m_t.c, temp.size()});
			}
			else
				temp.push_back({m_t.a, m_t.b, m_t.c, temp.size()});
		}

		T = temp;
	}
	node_triangle_share_sweep();
}

//updated on 23/8/18
void mesh::edge_swap()
{
	uint64_t flag = 0;
	edge_triangle_share_sweep();
	triangle_edge_share_sweep();
	do
	{
		flag = 0;
		for (edge &m_e : E)
		{
			if (m_e.location == edge_location::inside)
			{
				double min_old_1, min_new_1, min_old_2, min_new_2;
				min_old_1 = triangle_min_angle(m_e.T[0]);
				min_old_2 = triangle_min_angle(m_e.T[1]);

				node_id a, b;
				a = vertex_opposite_to_triangle_edge(T[m_e.T[0]], m_e);
				b = vertex_opposite_to_triangle_edge(T[m_e.T[1]], m_e);

				min_new_1 = min_angle_of_triangle(N[m_e.start].p, N[a].p, N[b].p);
				min_new_2 = min_angle_of_triangle(N[m_e.end].p, N[a].p, N[b].p);

				double area_old_1, area_old_2, area_new_1, area_new_2;
				area_old_1 = triangle_area(T[m_e.T[0]]);
				area_old_2 = triangle_area(T[m_e.T[1]]);

				area_new_1 = area_of_triangle(N[m_e.start].p, N[a].p, N[b].p);
				area_new_2 = area_of_triangle(N[m_e.end].p, N[a].p, N[b].p);

				if (min(min_old_1, min_old_2) < min(min_new_1, min_new_2) && (area_new_1 > epsilon && area_new_2 > epsilon) && //
					fabs(area_new_1 + area_new_2 - (area_old_1 + area_old_2)) < epsilon)
				{
					edge_id edge_to_be_updated_0, edge_to_be_updated_1;
					for (const edge_id e_id : T[m_e.T[0]].E)
					{
						if (m_e.id != e_id && (E[e_id].start == m_e.start || E[e_id].end == m_e.start))
						{
							edge_to_be_updated_0 = e_id;
							break;
						}
					}

					for (const edge_id e_id : T[m_e.T[1]].E)
					{
						if (m_e.id != e_id && (E[e_id].end == m_e.end || E[e_id].start == m_e.end))
						{
							edge_to_be_updated_1 = e_id;
							break;
						}
					}

					for (triangle_id &t_id : E[edge_to_be_updated_0].T)
						if (t_id == m_e.T[0])
							t_id = m_e.T[1];

					for (triangle_id &t_id : E[edge_to_be_updated_1].T)
						if (t_id == m_e.T[1])
							t_id = m_e.T[0];

					T[m_e.T[0]].a = a;
					T[m_e.T[0]].b = b;
					T[m_e.T[0]].c = m_e.end;

					T[m_e.T[1]].a = a;
					T[m_e.T[1]].b = b;
					T[m_e.T[1]].c = m_e.start;

					m_e.start = a;
					m_e.end = b;

					for (edge_id &e_id : T[m_e.T[0]].E)
						if (e_id == edge_to_be_updated_0)
							e_id = edge_to_be_updated_1;

					for (edge_id &e_id : T[m_e.T[1]].E)
						if (e_id == edge_to_be_updated_1)
							e_id = edge_to_be_updated_0;

					flag++;
				}
			}
		}

	} while (flag != 0);
	node_triangle_share_sweep();
	node_edge_share_sweep();
}

//updated on 23/8/18
void mesh::centroid_shift()
{
	for (node &m_n : N)
	{
		pos centroid;
		if (m_n.location == node_location::inside)
		{
			unordered_set<node_id> S;
			vector<node> temp;
			for (const triangle_id t_id : m_n.T)
			{
				S.insert(T[t_id].a);
				S.insert(T[t_id].b);
				S.insert(T[t_id].c);
			}

			for (const node_id n_id : S)
				temp.push_back(N[n_id]);

			m_n.p = generate_centroid(temp);
		}
	}
}

void mesh::generate_mesh_full()
{
	generate_mesh_basic();
	node_insertion();
	refine_triangles();
	edge_swap();
	refine_triangles_near_boundary(node_location::hole);
	refine_triangles_near_boundary(node_location::boundary);
	edge_swap();
	refine_triangles_near_boundary(node_location::hole);
	refine_triangles_near_boundary(node_location::boundary);
	edge_swap();
	refine_triangles();
	edge_swap();
	centroid_shift();
	node_insertion();
	edge_swap();
	centroid_shift();
	centroid_shift();
}

//updated on 23/8/18
void mesh::generate_ghosts()
{
	if (!ghost_generated)
	{
		edge_triangle_share_sweep();

		for (edge &m_e : E)
			if (m_e.location == edge_location::boundary)
			{
				node_id n_id = vertex_opposite_to_triangle_edge(T[m_e.T[0]], m_e);
				pos p = generate_ghost_point(m_e.T[0], n_id);
				N.push_back({p, N.size(), node_location::outside, false});
				make_triangle(m_e.start, m_e.end, N[N.size() - 1].id, triangle_type::ghost);
			}
	}
	ghost_generated = true;
}

//updated on 23/8/18
void mesh::display(bool inspect, bool swap_buffer)
{
	p = {0, 0, 0};
	dx = 0.05;
	dy = 0.05;
	dz = 0.05;
	zoom = 1.0;
	zoom_change = 1.1;
	return_to_console = false;

	if (type == mesh_type::_2d)
	{

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
				node_size[i] = 8.0;
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

		shadersource src = parseshader("src/shaders/display_2d.glsl");
		unsigned int shader = createshader(src.vertex, src.fragment);
		glUseProgram(shader);

		glEnable(GL_PROGRAM_POINT_SIZE);

		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
		glm::mat4 mvp = trans * scale;

		uniform MVP(shader, "MVP", uniform_types::MAT4F, (void *)&mvp[0][0]);
		uniform psize(shader, "psize", uniform_types::FLOAT, (void *)&zoom);

		if (!inspect && !swap_buffer)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			va_node.draw();
			va_triangle_edge.draw();

			trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
			scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
			mvp = trans * scale;

			MVP.update();
			psize.update();

			glfwSwapBuffers(window);
		}

		else if (inspect && !swap_buffer)
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

				glfwSwapBuffers(window);
				glfwPollEvents();
			}
		}

		else if (!inspect && swap_buffer)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			va_node.draw();
			va_triangle_edge.draw();

			trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
			scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
			mvp = trans * scale;

			MVP.update();
			psize.update();
		}

		glDeleteProgram(shader);
	}
}

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
int64_t mesh::edge_exists(const vector<edge> &E, const edge &e)
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
	for (size_t i = 0; i < m_N.size(); ++i)
	{
		node_index[i] = GLuint(m_N[i].id);
	}
	for (const node &n : N)
	{
		node_color.push_back(colors("yellow"));
		node_size.push_back(12.0);
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
}

void mesh::display_edge(const vector<edge> &m_E)
{
	vector<GLuint> edge_index(2 * m_E.size());
	/*unordered_set<uint64_t> unique_nodes;
	for(const edge& e:m_E)
	{
		unique_nodes.insert(e.start);
		unique_nodes.insert(e.end);
	}
	uint64_t n = unique_nodes.size();
	vector<float> posdata;
	for(const uint64_t value:unique_nodes)
	{
		posdata.push_back((float)N[value].p.x);
		posdata.push_back((float)N[value].p.y);
		posdata.push_back((float)N[value].p.z);
	}*/

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
		for (size_t i = 0; i < number_of_nodes(); ++i)
		{
			node_index[i] = GLuint(i);
			switch (N[i].location)
			{
			case node_location::boundary:
			{
				node_color[i] = colors("green");
				node_size[i] = 8.0;
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