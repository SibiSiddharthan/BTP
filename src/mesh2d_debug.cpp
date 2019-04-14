//Only compiles if this macro is defined
#ifdef MESH_DEBUG

#include "mesh2d.h"
#include "gl_abstraction.h"
using namespace std;

namespace __2d__
{
void mesh::generate_mesh_basic_debug()
{
	window w(800, 800);
	bool debug = true, step = false;

	edge e1, e2;
	int64_t e_id;
	double min_dist = 1.0e10;
	double temp;
	int k = -1;
	while (!w.should_close())
	{
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

				if (E[i].availability && k > -1)
				{
					if (debug)
					{
						step = false;
						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						while (!step)
						{
							ImGui_ImplOpenGL3_NewFrame();
							ImGui_ImplGlfw_NewFrame();
							ImGui::NewFrame();

							glClear(GL_COLOR_BUFFER_BIT);
							display_internal(w);
							display_node({N[k], N[E[i].start], N[E[i].end]});
							display_edge({E[i]});

							ImGui::Begin("button");
							if (ImGui::Button("step"))
							{
								step = true;
							}
							ImGui::SameLine();
							if (ImGui::Button("skip"))
							{
								debug = false;
								step = true;
							}

							ImGui::End();
							ImGui::Render();
							ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

							w.swap_buffers();
							w.poll_events();
							if (w.should_close())
							{
								debug = false;
								step = true;
							}
						}
					}
					e1.start = N[k].id;
					e1.end = E[i].end;
					e2.start = E[i].start;
					e2.end = N[k].id;

					e_id = edge_exists(e1);
					if (e_id != -1)
					{
						E[e_id].availability = false;
						e1.id = e_id;
						disable_common_node(e1, E[i]);
					}

					else
						make_inside_edge(e1.start, e1.end, true);

					e_id = edge_exists(e2);
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

					if (debug)
					{
						step = false;
						glEnable(GL_PROGRAM_POINT_SIZE);
						glEnable(GL_LINE_SMOOTH);
						while (!step)
						{
							ImGui_ImplOpenGL3_NewFrame();
							ImGui_ImplGlfw_NewFrame();
							ImGui::NewFrame();

							glClear(GL_COLOR_BUFFER_BIT);
							display_internal(w);
							display_node({N[k], N[E[i].start], N[E[i].end]});
							display_triangle({T[T.size() - 1]});

							ImGui::Begin("button");
							if (ImGui::Button("step"))
							{
								step = true;
							}
							ImGui::SameLine();
							if (ImGui::Button("skip"))
							{
								debug = false;
								step = true;
							}
							ImGui::End();
							ImGui::Render();
							ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

							w.swap_buffers();
							w.poll_events();

							if (w.should_close())
							{
								debug = false;
								step = true;
							}
						}
					}
				}
			}
		}
		if (!w.should_close())
		{
			glClear(GL_COLOR_BUFFER_BIT);
			display_internal(w);
			w.swap_buffers();
			w.poll_events();
		}
	}
}

//updated on 23/8/18
void mesh::node_insertion_debug()
{
	window w(800, 800);
	bool debug = true, step = false;

	while (!w.should_close())
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
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								display_node(temp);
								vector<triangle> T_temp;
								for (const uint64_t &t_id : N[i].T)
									T_temp.push_back(T[t_id]);
								display_triangle(T_temp);

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}

								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();
								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
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
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								display_node(temp);
								vector<triangle> T_temp;
								for (const uint64_t &t_id : N[N.size()-1].T)
									T_temp.push_back(T[t_id]);
								display_triangle(T_temp);

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}

								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();
								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
						}
					}
				}
			}
		}
		if (!w.should_close())
		{
			glClear(GL_COLOR_BUFFER_BIT);
			display_internal(w);
			w.swap_buffers();
			w.poll_events();
		}
	}
}

//updated on 23/8/18
void mesh::refine_triangles_debug()
{
	window w(800, 800);
	bool debug = true, step = false;

	double avg = avg_area_of_triangles();

	uint64_t flag = 0;
	if (avg > area_threshold)
	{
		while (!w.should_close())
		{
			do
			{
				vector<triangle> temp;
				flag = 0;
				for (const triangle &m_t : T)
				{
					if (double area = triangle_area(m_t); area > 2 * avg && area > 2.5 * area_threshold)
					{
						if (debug)
						{
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								display_triangle({m_t});

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}
								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();

								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
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
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								display_triangle({temp[temp.size() - 1], temp[temp.size() - 2], temp[temp.size() - 3]});

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}
								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();

								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
						}
					}
					else
						temp.push_back({m_t.a, m_t.b, m_t.c, temp.size()});
				}

				T = temp;
				for (const triangle &m_t : T)
					if (triangle_area(m_t) > (2 * avg))
						++flag;
			} while (flag != 0);

			if (!w.should_close())
			{
				glClear(GL_COLOR_BUFFER_BIT);
				display_internal(w);
				w.swap_buffers();
				w.poll_events();
			}
		}
	}
	node_triangle_share_sweep();
}

//updated on 23/8/18
void mesh::refine_triangles_near_boundary_debug(node_location _location)
{
	window w(800, 800);
	bool debug = true, step = false;

	double avg = avg_area_of_triangles_near_boundary(_location);

	if (avg > area_threshold)
	{
		while (!w.should_close())
		{
			vector<triangle> temp;
			for (const triangle &m_t : T)
			{
				if (N[m_t.a].location == _location || N[m_t.b].location == _location || N[m_t.c].location == _location)
				{
					if (double area = triangle_area(m_t); area > 1.5 * avg && area > 2 * area_threshold)
					{
						if (debug)
						{
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								display_triangle({m_t});

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}
								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();

								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
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
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								display_triangle({temp[temp.size() - 1], temp[temp.size() - 2], temp[temp.size() - 3]});

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}
								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();

								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
						}
					}
					else
						temp.push_back({m_t.a, m_t.b, m_t.c, temp.size()});
				}
				else
					temp.push_back({m_t.a, m_t.b, m_t.c, temp.size()});
			}

			T = temp;
			if (!w.should_close())
			{
				glClear(GL_COLOR_BUFFER_BIT);
				display_internal(w);
				w.swap_buffers();
				w.poll_events();
			}
		}
	}
	node_triangle_share_sweep();
}

//updated on 23/8/18
void mesh::edge_swap_debug()
{
	window w(800, 800);
	bool debug = true, step = false;

	uint64_t flag = 0;
	edge_triangle_share_sweep();
	triangle_edge_share_sweep();
	while (!w.should_close())
	{
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
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								vector<triangle> T_temp;
								for (const uint64_t &t_id : m_e.T)
									T_temp.push_back(T[t_id]);
								display_triangle(T_temp);

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}
								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();

								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
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
							step = false;
							glEnable(GL_PROGRAM_POINT_SIZE);
							glEnable(GL_LINE_SMOOTH);
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);
								display_internal(w);
								vector<triangle> T_temp;
								for (const uint64_t &t_id : m_e.T)
									T_temp.push_back(T[t_id]);
								display_triangle(T_temp);

								ImGui::Begin("button");
								if (ImGui::Button("step"))
								{
									step = true;
								}
								ImGui::SameLine();
								if (ImGui::Button("skip"))
								{
									debug = false;
									step = true;
								}
								ImGui::End();
								ImGui::Render();
								ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

								w.swap_buffers();
								w.poll_events();

								if (w.should_close())
								{
									debug = false;
									step = true;
								}
							}
						}
					}
				}
			}

		} while (flag != 0);
		if (!w.should_close())
		{
			glClear(GL_COLOR_BUFFER_BIT);
			display_internal(w);
			w.swap_buffers();
			w.poll_events();
		}
	}
	node_triangle_share_sweep();
	node_edge_share_sweep();
}

//updated on 23/8/18
void mesh::centroid_shift_debug()
{
	window w(800, 800);
	bool debug = true, step = false;
	while (!w.should_close())
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
					step = false;
					glEnable(GL_PROGRAM_POINT_SIZE);
					glEnable(GL_LINE_SMOOTH);
					while (!step)
					{
						ImGui_ImplOpenGL3_NewFrame();
						ImGui_ImplGlfw_NewFrame();
						ImGui::NewFrame();

						glClear(GL_COLOR_BUFFER_BIT);
						display_internal(w);
						temp.push_back(m_n);
						display_node(temp);
						vector<triangle> T_temp;
						for (const uint64_t &t_id : m_n.T)
							T_temp.push_back(T[t_id]);
						display_triangle(T_temp);

						ImGui::Begin("button");
						if (ImGui::Button("step"))
						{
							step = true;
						}
						ImGui::SameLine();
						if (ImGui::Button("skip"))
						{
							debug = false;
							step = true;
						}
						ImGui::End();
						ImGui::Render();
						ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

						w.swap_buffers();
						w.poll_events();

						if (w.should_close())
						{
							debug = false;
							step = true;
						}
					}
					temp.push_back(m_n);
					display_node(temp);
				}
				m_n.p = generate_centroid(temp);

				if (debug)
				{
					step = false;
					glEnable(GL_PROGRAM_POINT_SIZE);
					glEnable(GL_LINE_SMOOTH);
					while (!step)
					{
						ImGui_ImplOpenGL3_NewFrame();
						ImGui_ImplGlfw_NewFrame();
						ImGui::NewFrame();

						glClear(GL_COLOR_BUFFER_BIT);
						display_internal(w);
						temp.push_back(m_n);
						display_node(temp);
						vector<triangle> T_temp;
						for (const uint64_t &t_id : m_n.T)
							T_temp.push_back(T[t_id]);
						display_triangle(T_temp);

						ImGui::Begin("button");
						if (ImGui::Button("step"))
						{
							step = true;
						}
						ImGui::SameLine();
						if (ImGui::Button("skip"))
						{
							debug = false;
							step = true;
						}
						ImGui::End();
						ImGui::Render();
						ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

						w.swap_buffers();
						w.poll_events();

						if (w.should_close())
						{
							debug = false;
							step = true;
						}
					}
					temp.push_back(m_n);
					display_node(temp);
				}
			}
		}
		if (!w.should_close())
		{
			glClear(GL_COLOR_BUFFER_BIT);
			display_internal(w);
			w.swap_buffers();
			w.poll_events();
		}
	}
}
} // namespace __2d__

#endif