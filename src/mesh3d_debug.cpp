//Only compiles if this macro is defined
#ifdef MESH_DEBUG

#include "mesh3d.h"
#include "gl_abstraction.h"
#include <fmt/core.h>
using namespace std;

namespace __3d__
{

void mesh::generate_mesh_basic_debug()
{
	window w(800, 800);
	bool debug = true, step = false;
	pos p;
	float rotx, roty;
	bool draw_normals = false;

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
	//glEnable(GL_DEPTH_TEST);

	plane p1, p2, p3;
	int64_t p_id;
	double min_dist = 1.0e10;
	double temp;
	int k = -1;
	while (!w.should_close())
	{
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
						if (debug)
						{
							step = false;
							bool display_state = true;
							bool left_test_pass = left_test(P[i], N[j]);
							bool coplanar_test_pass = coplanar_test(P[i], N[j]);
							string status = fmt::format("Left Test: {}\nCoplanar Test: {}",
														left_test_pass == 1 ? "pass" : "fail",
														coplanar_test_pass == 1 ? "pass" : "fail");
							string details = fmt::format("Left Test: {}\nCoplanar Test: {}",
														 left_test_debug(P[i], N[j]), coplanar_test_debug(P[i], N[j]));
							while (!step)
							{
								ImGui_ImplOpenGL3_NewFrame();
								ImGui_ImplGlfw_NewFrame();
								ImGui::NewFrame();

								glClear(GL_COLOR_BUFFER_BIT);

								p = w.get_pos();
								rotx = w.get_rotx();
								roty = w.get_roty();
								ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
								ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
								View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

								prog.update_uniforms();
								if (display_state)
									display_internal(w, draw_normals);
								display_nodes({N[j]});
								display_planes({P[i]}, draw_normals);

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
								ImGui::Checkbox("display state", &display_state);
								ImGui::Checkbox("draw_normals", &draw_normals);
								ImGui::SetWindowPos(ImVec2(0, 0));
								ImGui::Text(status.c_str());
								ImGui::Text(details.c_str());
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

						if (left_test(P[i], N[j]) && coplanar_test(P[i], N[j]))
						{
							//Have to check
							p1.a = P[i].b;
							p1.b = P[i].a;
							p1.c = N[j].id;

							p2.a = N[j].id;
							p2.b = P[i].c;
							p2.c = P[i].b;

							p3.a = P[i].c;
							p3.b = N[j].id;
							p3.c = P[i].a;

							if (debug)
							{
								step = false;
								bool display_state = true;
								bool p1_intersection = intersection_test(p1, P);
								bool p2_intersection = intersection_test(p2, P);
								bool p3_intersection = intersection_test(p3, P);
								string status = fmt::format("p1:{} p2:{} p3:{}",
															p1_intersection == 1 ? "pass" : "fail",
															p2_intersection == 1 ? "pass" : "fail",
															p3_intersection == 1 ? "pass" : "fail");
								bool display_p1 = true, display_p2 = true, display_p3 = true;
								while (!step)
								{
									ImGui_ImplOpenGL3_NewFrame();
									ImGui_ImplGlfw_NewFrame();
									ImGui::NewFrame();

									glClear(GL_COLOR_BUFFER_BIT);

									p = w.get_pos();
									rotx = w.get_rotx();
									roty = w.get_roty();
									ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
									ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
									View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

									prog.update_uniforms();
									if (display_state)
										display_internal(w, draw_normals);
									display_nodes({N[j]});
									display_planes({P[i]}, draw_normals);

									if (display_p1)
										display_planes({p1}, draw_normals,colors("green"));
									if (display_p2)
										display_planes({p2}, draw_normals,colors("green"));
									if (display_p3)
										display_planes({p3}, draw_normals,colors("green"));

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
									ImGui::Checkbox("display state", &display_state);
									ImGui::Checkbox("draw_normals", &draw_normals);
									ImGui::Checkbox("p1", &display_p1);
									ImGui::Checkbox("p2", &display_p2);
									ImGui::Checkbox("p3", &display_p3);
									ImGui::SetWindowPos(ImVec2(0, 0));
									ImGui::Text(status.c_str());
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
							
							if (intersection_test(p1, P) && intersection_test(p2, P) && intersection_test(p3, P))
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
					if (debug)
					{
						step = false;

						while (!step)
						{
							ImGui_ImplOpenGL3_NewFrame();
							ImGui_ImplGlfw_NewFrame();
							ImGui::NewFrame();

							glClear(GL_COLOR_BUFFER_BIT );

							p = w.get_pos();
							rotx = w.get_rotx();
							roty = w.get_roty();
							ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
							ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
							View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

							prog.update_uniforms();
							display_internal(w, draw_normals);
							//display_node({N[k], N[E[i].start], N[E[i].end]});
							//display_edge({E[i]});

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
					//Have to check
					p1.a = P[i].b;
					p1.b = P[i].a;
					p1.c = N[k].id;

					p2.a = N[k].id;
					p2.b = P[i].c;
					p2.c = P[i].b;

					p3.a = P[i].c;
					p3.b = N[k].id;
					p3.c = P[i].a;

					p_id = plane_exists(p1);
					if (p_id != -1)
					{
						P[p_id].availability = false;
						p1.id = p_id;
						disable_common_node(p1, P[i]);
					}

					else
						make_inside_plane(p1.a, p1.b, p1.c, true);

					p_id = plane_exists(p2);
					if (p_id != -1)
					{
						P[p_id].availability = false;
						p2.id = p_id;
						disable_common_node(p2, P[i]);
					}
					else
						make_inside_plane(p2.a, p2.b, p2.c, true);

					p_id = plane_exists(p3);
					if (p_id != -1)
					{
						P[p_id].availability = false;
						p3.id = p_id;
						disable_common_node(p3, P[i]);
					}

					else
						make_inside_plane(p3.a, p3.b, p3.c, true);

					make_tetrahedron(P[i].a, P[i].b, P[i].c, N[k].id);
					P[i].availability = false;

					if (debug)
					{
						step = false;
						while (!step)
						{
							ImGui_ImplOpenGL3_NewFrame();
							ImGui_ImplGlfw_NewFrame();
							ImGui::NewFrame();

							glClear(GL_COLOR_BUFFER_BIT );
							p = w.get_pos();
							rotx = w.get_rotx();
							roty = w.get_roty();
							ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
							ViewRotateX = glm::rotate(ViewTranslate, rotx, glm::vec3(-1.0f, 0.0f, 0.0f));
							View = glm::rotate(ViewRotateX, roty, glm::vec3(0.0f, 1.0f, 0.0f));

							prog.update_uniforms();
							display_internal(w, draw_normals);
							//display_node({N[k], N[E[i].start], N[E[i].end]});
							//display_edge({E[i]});

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
					//cout << number_of_unused_planes(P);
					//return;
				}
			}
		}
		if (!w.should_close())
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			display_internal(w, draw_normals);
			w.swap_buffers();
			w.poll_events();
		}
	}
}
} // namespace __3d__

#endif