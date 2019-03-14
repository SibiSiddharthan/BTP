#include "mesh.h"

/*
This file contains the definitions of public functions used class mesh
except mesh::display()
*/

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
				update_pdata();

				shadersource src = parseshader("src/shaders/debug_display.glsl");
				unsigned int shader = createshader(src.vertex, src.fragment);
				glUseProgram(shader);

				glEnable(GL_PROGRAM_POINT_SIZE);
				glEnable(GL_LINE_SMOOTH);
				glClear(GL_COLOR_BUFFER_BIT);
				display_all_nodes();
				display_all_edges();
				display_node({N[k], N[E[i].start], N[E[i].end]});
				glLineWidth(3.0);
				display_edge({E[i]});
				glLineWidth(1.0);
				window_swap_buffers();
				glDeleteProgram(shader);
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

			if (debug && k > -1)
			{
				glfwPostEmptyEvent();
				cin.get();
				update_pdata();

				shadersource src = parseshader("src/shaders/debug_display.glsl");
				unsigned int shader = createshader(src.vertex, src.fragment);
				glUseProgram(shader);

				glEnable(GL_PROGRAM_POINT_SIZE);
				glEnable(GL_LINE_SMOOTH);
				glClear(GL_COLOR_BUFFER_BIT);
				display_all_nodes();
				display_all_edges();
				display_node({N[k], N[E[i].start], N[E[i].end]});
				glLineWidth(3.0);
				display_triangle({T[T.size() - 1]});
				glLineWidth(1.0);
				window_swap_buffers();
				glDeleteProgram(shader);
				glfwWaitEventsTimeout(100.0);
				cout << "Enter for next\n";
				cin.get();
			}
		}
	}
}

//updated on 23/8/18
void mesh::node_insertion(bool debug)
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
					if (debug)
					{
						glfwPostEmptyEvent();
						cin.get();
						update_pdata();

						shadersource src = parseshader("src/shaders/debug_display.glsl");
						unsigned int shader = createshader(src.vertex, src.fragment);
						glUseProgram(shader);

						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						glClear(GL_COLOR_BUFFER_BIT);
						display_all_nodes();
						display_all_edges();
						temp.push_back(N[i]);
						display_node(temp);
						glLineWidth(3.0);
						vector<mesh_triangle> T_temp;
						for (const uint64_t &t_id : N[i].T)
							T_temp.push_back(T[t_id]);
						display_triangle(T_temp);
						glLineWidth(1.0);
						window_swap_buffers();
						glDeleteProgram(shader);
						glfwWaitEventsTimeout(100.0);
						cout << "Enter for next\n";
						cin.get();
					}

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

					if (debug)
					{
						glfwPostEmptyEvent();
						cin.get();
						update_pdata();

						shadersource src = parseshader("src/shaders/debug_display.glsl");
						unsigned int shader = createshader(src.vertex, src.fragment);
						glUseProgram(shader);

						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						glClear(GL_COLOR_BUFFER_BIT);
						display_all_nodes();
						display_all_edges();
						temp.push_back(N[i]);
						display_node(temp);
						glLineWidth(3.0);
						vector<mesh_triangle> T_temp;
						for (const uint64_t &t_id : N[N.size() - 1].T)
							T_temp.push_back(T[t_id]);
						display_triangle(T_temp);
						glLineWidth(1.0);
						window_swap_buffers();
						glDeleteProgram(shader);
						glfwWaitEventsTimeout(100.0);
						cout << "Enter for next\n";
						cin.get();
					}
				}
			}
		}
	}
}

//updated on 23/8/18
void mesh::refine_triangles(bool debug)
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
					if (debug)
					{
						glfwPostEmptyEvent();
						cin.get();
						update_pdata();

						shadersource src = parseshader("src/shaders/debug_display.glsl");
						unsigned int shader = createshader(src.vertex, src.fragment);
						glUseProgram(shader);

						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						glClear(GL_COLOR_BUFFER_BIT);
						display_all_nodes();
						display_all_edges();
						glLineWidth(3.0);
						display_triangle({m_t});
						glLineWidth(1.0);
						window_swap_buffers();
						glDeleteProgram(shader);
						glfwWaitEventsTimeout(100.0);
						cout << "Enter for next\n";
						cin.get();
					}
					N.push_back({(N[m_t.a].p + N[m_t.b].p + N[m_t.c].p) / 3.0, N.size(), node_location::inside, false});

					temp.push_back({N[N.size() - 1].id, m_t.b, m_t.c, temp.size()});
					temp.push_back({m_t.a, N[N.size() - 1].id, m_t.c, temp.size()});
					temp.push_back({m_t.a, m_t.b, N[N.size() - 1].id, temp.size()});

					make_inside_edge(N[N.size() - 1].id, m_t.a, false);
					make_inside_edge(N[N.size() - 1].id, m_t.b, false);
					make_inside_edge(N[N.size() - 1].id, m_t.c, false);

					if (debug)
					{
						glfwPostEmptyEvent();
						cin.get();
						update_pdata();

						shadersource src = parseshader("src/shaders/debug_display.glsl");
						unsigned int shader = createshader(src.vertex, src.fragment);
						glUseProgram(shader);

						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						glClear(GL_COLOR_BUFFER_BIT);
						display_all_nodes();
						display_all_edges();
						glLineWidth(3.0);
						display_triangle({temp[temp.size() - 1], temp[temp.size() - 2], temp[temp.size() - 3]});
						glLineWidth(1.0);
						window_swap_buffers();
						glDeleteProgram(shader);
						glfwWaitEventsTimeout(100.0);
						cout << "Enter for next\n";
						cin.get();
					}
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
void mesh::edge_swap(bool debug)
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

					if (debug)
					{
						glfwPostEmptyEvent();
						cin.get();
						update_pdata();

						shadersource src = parseshader("src/shaders/debug_display.glsl");
						unsigned int shader = createshader(src.vertex, src.fragment);
						glUseProgram(shader);

						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						glClear(GL_COLOR_BUFFER_BIT);
						display_all_nodes();
						display_all_edges();
						glLineWidth(3.0);
						vector<mesh_triangle> T_temp;
						for (const uint64_t &t_id : m_e.T)
							T_temp.push_back(T[t_id]);
						display_triangle(T_temp);
						glLineWidth(1.0);
						window_swap_buffers();
						glDeleteProgram(shader);
						glfwWaitEventsTimeout(100.0);
						cout << "Enter for next\n";
						cin.get();
					}
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

					if (debug)
					{
						glfwPostEmptyEvent();
						cin.get();
						update_pdata();

						shadersource src = parseshader("src/shaders/debug_display.glsl");
						unsigned int shader = createshader(src.vertex, src.fragment);
						glUseProgram(shader);

						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						glClear(GL_COLOR_BUFFER_BIT);
						display_all_nodes();
						display_all_edges();
						glLineWidth(3.0);
						vector<mesh_triangle> T_temp;
						for (const uint64_t &t_id : m_e.T)
							T_temp.push_back(T[t_id]);
						display_triangle(T_temp);
						glLineWidth(1.0);
						window_swap_buffers();
						glDeleteProgram(shader);
						glfwWaitEventsTimeout(100.0);
						cout << "Enter for next\n";
						cin.get();
					}
				}
			}
		}

	} while (flag != 0);
	node_triangle_share_sweep();
	node_edge_share_sweep();
}

//updated on 23/8/18
void mesh::centroid_shift(bool debug)
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

			if (debug)
			{
				glfwPostEmptyEvent();
				cin.get();
				update_pdata();

				shadersource src = parseshader("src/shaders/debug_display.glsl");
				unsigned int shader = createshader(src.vertex, src.fragment);
				glUseProgram(shader);

				glEnable(GL_PROGRAM_POINT_SIZE);
				glEnable(GL_LINE_SMOOTH);
				glClear(GL_COLOR_BUFFER_BIT);
				display_all_nodes();
				display_all_edges();
				temp.push_back(m_n);
				display_node(temp);
				glLineWidth(3.0);
				vector<mesh_triangle> T_temp;
				for (const uint64_t &t_id : m_n.T)
					T_temp.push_back(T[t_id]);
				display_triangle(T_temp);
				glLineWidth(1.0);
				window_swap_buffers();
				glDeleteProgram(shader);
				glfwWaitEventsTimeout(100.0);
				cout << "Enter for next\n";
				cin.get();
			}
			m_n.p = generate_centroid(temp);

			if (debug)
			{
				glfwPostEmptyEvent();
				cin.get();
				update_pdata();

				shadersource src = parseshader("src/shaders/debug_display.glsl");
				unsigned int shader = createshader(src.vertex, src.fragment);
				glUseProgram(shader);

				glEnable(GL_PROGRAM_POINT_SIZE);
				glEnable(GL_LINE_SMOOTH);
				glClear(GL_COLOR_BUFFER_BIT);
				display_all_nodes();
				display_all_edges();
				temp.push_back(m_n);
				display_node(temp);
				glLineWidth(3.0);
				vector<mesh_triangle> T_temp;
				for (const uint64_t &t_id : m_n.T)
					T_temp.push_back(T[t_id]);
				display_triangle(T_temp);
				glLineWidth(1.0);
				window_swap_buffers();
				glDeleteProgram(shader);
				glfwWaitEventsTimeout(100.0);
				cout << "Enter for next\n";
				cin.get();
			}
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

void mesh::refine_tetrahedrons()
{
	double avg = avg_volume_of_tetrahedrons();

	if (avg > volume_threshold)
	{
		vector<tetrahedron> temp;
		for (const tetrahedron &m_th : TH)
		{
			if (double volume = tetrahedron_volume(m_th); volume > 1.5 * avg && volume > 2 * volume_threshold)
			{

				N.push_back({(N[m_th.a].p + N[m_th.b].p + N[m_th.c].p + N[m_th.d].p) / 4.0, N.size(), node_location::inside, false});

				temp.push_back({N[N.size() - 1].id, m_th.b, m_th.c, m_th.d, temp.size()});
				temp.push_back({m_th.a, N[N.size() - 1].id, m_th.c, m_th.d, temp.size()});
				temp.push_back({m_th.a, m_th.b, N[N.size() - 1].id, m_th.d, temp.size()});
				temp.push_back({m_th.a, m_th.b, m_th.c, N[N.size() - 1].id, temp.size()});

				make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.b, false);
				make_inside_plane(N[N.size() - 1].id, m_th.b, m_th.c, false);
				make_inside_plane(N[N.size() - 1].id, m_th.c, m_th.d, false);
				make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.d, false);
			}
			else
				temp.push_back({m_th.a, m_th.b, m_th.c, m_th.d, temp.size()});
		}

		TH = temp;
	}
	node_tetrahedron_share_sweep();
}

void mesh::refine_tetrahedrons_near_boundary(node_location _location)
{
	double avg = avg_volume_of_tetrahedrons_near_boundary(_location);

	if (avg > volume_threshold)
	{
		vector<tetrahedron> temp;
		for (const tetrahedron &m_th : TH)
		{
			if (N[m_th.a].location == _location || N[m_th.b].location == _location || N[m_th.c].location == _location || N[m_th.d].location == _location)
			{
				if (double volume = tetrahedron_volume(m_th); volume > 1.5 * avg && volume > 2 * volume_threshold)
				{

					N.push_back({(N[m_th.a].p + N[m_th.b].p + N[m_th.c].p + N[m_th.d].p) / 4.0, N.size(), node_location::inside, false});

					temp.push_back({N[N.size() - 1].id, m_th.b, m_th.c, m_th.d, temp.size()});
					temp.push_back({m_th.a, N[N.size() - 1].id, m_th.c, m_th.d, temp.size()});
					temp.push_back({m_th.a, m_th.b, N[N.size() - 1].id, m_th.d, temp.size()});
					temp.push_back({m_th.a, m_th.b, m_th.c, N[N.size() - 1].id, temp.size()});

					make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.b, false);
					make_inside_plane(N[N.size() - 1].id, m_th.b, m_th.c, false);
					make_inside_plane(N[N.size() - 1].id, m_th.c, m_th.d, false);
					make_inside_plane(N[N.size() - 1].id, m_th.a, m_th.d, false);
				}
				else
					temp.push_back({m_th.a, m_th.b, m_th.c, m_th.d, temp.size()});
			}
			else
				temp.push_back({m_th.a, m_th.b, m_th.c, m_th.d, temp.size()});
		}

		TH = temp;
	}
	node_tetrahedron_share_sweep();
}

void mesh::centroid_shift_3d()
{
	for (node &m_n : N)
	{
		pos centroid;
		if (m_n.location == node_location::inside)
		{
			unordered_set<node_id> S;
			vector<node> temp;
			for (const tetrahedron_id th_id : m_n.TH)
			{
				S.insert(TH[th_id].a);
				S.insert(TH[th_id].b);
				S.insert(TH[th_id].c);
				S.insert(TH[th_id].d);
			}

			for (const node_id n_id : S)
				temp.push_back(N[n_id]);

			m_n.p = generate_centroid(temp);
		}
	}
}

void mesh::node_insertion_3d()
{
	for (size_t i = 0; i < number_of_nodes(); i++)
	{
		if (N[i].location == node_location::boundary || N[i].location == node_location::hole)
		{
			if (N[i].tetrahedron_share() > 4) //number to be determined
			{
				unordered_set<node_id> S;
				vector<node> temp;
				pos centroid;
				pair<node_id, node_id> np;
				for (const tetrahedron_id th_id : N[i].TH)
				{
					S.insert(TH[th_id].a);
					S.insert(TH[th_id].b);
					S.insert(TH[th_id].c);
					S.insert(TH[th_id].d);
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

					N[i].TH.clear();
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

void mesh::generate_mesh_basic_3d()
{
	mesh_plane p1, p2, p3;
	int64_t p_id;
	double min_dist = 1.0e10;
	double temp;
	int k = -1;
	while (number_of_unused_planes(P) != 0)
	{
		for (size_t i = 0; i < number_of_planes(); i++)
		{
			k = -1;
			min_dist = 1.0e10;
			for (size_t j = 0; j < number_of_nodes(); j++)
			{
				if (P[i].availability && N[j].availability && (P[i].a != N[j].id && P[i].b != N[j].id && P[i].c != N[j].id))
				{
					if (left_test(P[i], N[j]) && collinear_test(P[i], N[j]))
					{

						//Have to check
						p1.a = N[j].id;
						p1.b = P[i].a;
						p1.c = P[i].b;

						p2.b = N[j].id;
						p2.a = P[i].c;
						p2.c = P[i].b;

						p3.c = N[j].id;
						p3.a = P[i].a;
						p3.b = P[i].c;

						if (true)//intersection_test(e1, E) && intersection_test(e2, E))
						{
							temp = distance((N[P[i].a].p + N[P[i].b].p + N[P[i].c].p) * 0.333, N[j].p);
							if (temp < min_dist)
							{
								k = j;
								min_dist = temp;
							}
						}
					}
				}
			}

			if (P[i].availability && k > -1)
			{
				//Have to check
				p1.a = N[k].id;
				p1.b = P[i].a;
				p1.c = P[i].b;

				p2.b = N[k].id;
				p2.a = P[i].c;
				p2.c = P[i].b;

				p3.c = N[k].id;
				p3.a = P[i].a;
				p3.b = P[i].c;

				p_id = plane_exists(P, p1);
				if (p_id != -1)
				{
					P[p_id].availability = false;
					p1.id = p_id;
					//disable_common_node(e1, E[i]);
				}

				else
					make_inside_plane(p1.a, p1.b,p1.c, true);

				p_id = plane_exists(P, p2);
				if (p_id != -1)
				{
					P[p_id].availability = false;
					p2.id = p_id;
					//disable_common_node(e2, E[i]);
				}
				else
					make_inside_plane(p2.a, p2.b,p2.c, true);
				
				p_id = plane_exists(P, p3);
				if (p_id != -1)
				{
					P[p_id].availability = false;
					p3.id = p_id;
					//disable_common_node(e2, E[i]);
				}

				else
					make_inside_plane(p3.a, p3.b,p3.c, true);


				//make_tetrahedron(E[i].start, E[i].end, N[k].id);
				P[i].availability = false;
			}
		}
	}
}