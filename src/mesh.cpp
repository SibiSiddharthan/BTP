#include "mesh.h"

bool return_to_console;
bool cursor_inside;
bool find_value;

pos p;
double dx;
double dy;
double dz;
double zoom;
double zoom_change;

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
	/*
	int count = 0;
	for (int i = 0; i < number_of_edges; i++)
		if (E[i].location == edge_location::boundary)
			++count;
	cout << "Number of edges on boundary: " << count << endl;
	*/
}

//updated on 23/8/18

void mesh::generate_mesh_basic()
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

						if (intersection_test(e1, E) && intersection_test(e2, E)) /*&& !enclosed_node(E[i].start->p, E[i].end->p, N[j].p, N, number_of_nodes)
							&& !edge_contains_other_nodes(e1, N, number_of_nodes) && !edge_contains_other_nodes(e2, N, number_of_nodes))*/
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
				//cout << area_of_triangle(T[nm - 1]) << endl;
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

					/*
					!need to make it work
					while(N[i].triangle_share()!=0)
					{
						triangle_node_change(N[i].T[T.size()-1], N[i].id, N[N.size() - 1].id);
						N[i].T.pop_back();
					}
					*/
					/*
					for (size_t j = N[i].triangle_share() - 1 ;j>=0 ; j--)
					{
						triangle_node_change(N[i].T[j], N[i].id, N[N.size() - 1].id);
						//N[i].T.pop_back();
					}
					*/
					for (size_t j = 0; j < N[i].triangle_share(); j++)
						triangle_node_change(N[i].T[j], N[i].id, N[N.size() - 1].id);

					for (size_t j = 0; j < N[i].edge_share(); j++)
						edge_node_change(N[i].IE[j], N[i].id, N[N.size() - 1].id);

					N[i].T.clear();
					N[i].IE.clear();

					//N[i].IE = N[i].BE;

					make_triangle(N[N.size() - 1].id, N[i].id, np.first);
					make_triangle(N[N.size() - 1].id, N[i].id, np.second);
					make_inside_edge(N[N.size()-1].id,N[i].id,false);
					make_inside_edge(N[N.size()-1].id,N[np.first].id,false);
					make_inside_edge(N[N.size()-1].id,N[np.second].id,false);

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
		//do
		//{
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
					temp.push_back(m_t);
			}

			T = temp;

			for (const mesh_triangle &m_t : T)
				if (triangle_area(m_t) > (2 * avg))
					++flag;

		//} while (flag != 0);
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
					temp.push_back(m_t);
			}
			else
				temp.push_back(m_t);
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
		for(edge & m_e:E)
		{
			if(m_e.location == edge_location::inside)
			{
				double min_old_1,min_new_1,min_old_2,min_new_2;
				min_old_1 = triangle_min_angle(m_e.T[0]);
				min_old_2 = triangle_min_angle(m_e.T[1]);

				node_id a,b;
				a = vertex_opposite_to_triangle_edge(T[m_e.T[0]],m_e);
				b = vertex_opposite_to_triangle_edge(T[m_e.T[1]],m_e);

				min_new_1 = min_angle_of_triangle(N[m_e.start].p,N[a].p,N[b].p);
				min_new_2 = min_angle_of_triangle(N[m_e.end].p,N[a].p,N[b].p);

				double area_old_1,area_old_2,area_new_1,area_new_2;
				area_old_1 = triangle_area(T[m_e.T[0]]);
				area_old_2 = triangle_area(T[m_e.T[1]]);

				area_new_1 = area_of_triangle(N[m_e.start].p,N[a].p,N[b].p);
				area_new_2 = area_of_triangle(N[m_e.end].p,N[a].p,N[b].p);

				if(min(min_old_1, min_old_2) < min(min_new_1, min_new_2) && (area_new_1 > epsilon && area_new_2 > epsilon)
				&& fabs(area_new_1 + area_new_2 - (area_old_1 + area_old_2))<epsilon)
				{
					edge_id edge_to_be_updated_0,edge_to_be_updated_1;
					for(const edge_id e_id : T[m_e.T[0]].E)
					{
						if(m_e.id != e_id && E[e_id].start == m_e.start)
						{
							edge_to_be_updated_0 = e_id;
							break;
						} 
					}
				
					for(const edge_id e_id : T[m_e.T[1]].E)
					{
						if(m_e.id != e_id && E[e_id].end == m_e.end)
						{
							edge_to_be_updated_1 = e_id;
							break;
						} 
					}

					for(triangle_id &t_id : E[edge_to_be_updated_0].T)
						if(t_id == m_e.T[0])
							t_id = m_e.T[1];

					for(triangle_id &t_id : E[edge_to_be_updated_1].T)
						if(t_id == m_e.T[1])
							t_id = m_e.T[0];

					T[m_e.T[0]].a = a;
					T[m_e.T[0]].b = b;
					T[m_e.T[0]].c = m_e.end;

					T[m_e.T[1]].a = a;
					T[m_e.T[1]].b = b;
					T[m_e.T[1]].c = m_e.start;

					m_e.start = a;
					m_e.end = b;

					for(edge_id &e_id : T[m_e.T[0]].E)
						if(e_id == edge_to_be_updated_0)
							e_id = edge_to_be_updated_1;

					for(edge_id &e_id : T[m_e.T[1]].E)
						if(e_id == edge_to_be_updated_1)
							e_id = edge_to_be_updated_0;

					flag++;
				}
			}
		}
	
	} while (flag != 0);
	node_triangle_share_sweep();

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

		for(edge& m_e : E)
			if(m_e.location == edge_location::boundary)
			{
				node_id n_id = vertex_opposite_to_triangle_edge(T[m_e.T[0]],m_e);
				pos p = generate_ghost_point(m_e.T[0],n_id);
				N.push_back({p,N.size(),node_location::outside,false});
				make_triangle(m_e.start,m_e.end,N[N.size()-1].id);
			}
	}
	ghost_generated = true;
}


//updated on 23/8/18
void mesh::imp_display()
{
	float *Edata, *Ndata;
	Edata = new float[6 * number_of_edges()];
	Ndata = new float[4 * number_of_nodes()];

	uint64_t k = 0;
	for (size_t i = 0; i < 6 * number_of_edges(); i += 6)
	{
		k = i / 6;
		Edata[i + 0] = (float)N[E[k].start].p.x;
		Edata[i + 1] = (float)N[E[k].start].p.y;
		Edata[i + 2] = (float)N[E[k].start].p.z;
		Edata[i + 3] = (float)N[E[k].end].p.x;
		Edata[i + 4] = (float)N[E[k].end].p.y;
		Edata[i + 5] = (float)N[E[k].end].p.z;
	}

	for (size_t i = 0; i < 4 * number_of_nodes(); i += 4)
	{
		k = i / 4;

		Ndata[i] = N[k].p.x;
		Ndata[i + 1] = N[k].p.y;
		Ndata[i + 2] = N[k].p.z;
		Ndata[i + 3] = 5.0;
	}

	GLuint bufedge, bufnode;
	unsigned int vao[2], test;

	glGenVertexArrays(2, vao);

	glBindVertexArray(vao[0]);
	glGenBuffers(1, &bufedge);
	glBindBuffer(GL_ARRAY_BUFFER, bufedge);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * number_of_edges(), Edata, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	glBindVertexArray(vao[1]);
	glGenBuffers(1, &bufnode);
	glBindBuffer(GL_ARRAY_BUFFER, bufnode);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * number_of_nodes(), Ndata, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));

	glBindVertexArray(0);

	shadersource src = parseshader("src/shaders/basic_display.glsl");
	unsigned int shader = createshader(src.vertex, src.fragment);
	glUseProgram(shader);

	glEnable(GL_PROGRAM_POINT_SIZE);

	{
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(vao[1]);
		glDrawArrays(GL_POINTS, 0, sizeof(float) * 4 * number_of_nodes() / (sizeof(float) * 4));

		glBindVertexArray(vao[0]);
		glDrawArrays(GL_LINES, 0, sizeof(float) * 6 * number_of_edges() / (sizeof(float) * 2));

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader);
	delete[] Edata;
	delete[] Ndata;
}

//updated on 23/8/18
void mesh::display()
{
	if (type == mesh_type::_2d)
	{

		float *Tdata, *Ndata;
		Tdata = new float[18 * number_of_triangles()];
		Ndata = new float[4 * number_of_nodes()];

		uint64_t k = 0;
		for (size_t i = 0; i < 18 * number_of_triangles(); i += 18)
		{
			k = i / 18;
			Tdata[i + 0] = (float)N[T[k].a].p.x;
			Tdata[i + 1] = (float)N[T[k].a].p.y;
			Tdata[i + 2] = (float)N[T[k].a].p.z;
			Tdata[i + 3] = (float)N[T[k].b].p.x;
			Tdata[i + 4] = (float)N[T[k].b].p.y;
			Tdata[i + 5] = (float)N[T[k].b].p.z;

			Tdata[i + 6] = (float)N[T[k].b].p.x;
			Tdata[i + 7] = (float)N[T[k].b].p.y;
			Tdata[i + 8] = (float)N[T[k].b].p.z;
			Tdata[i + 9] = (float)N[T[k].c].p.x;
			Tdata[i + 10] = (float)N[T[k].c].p.y;
			Tdata[i + 11] = (float)N[T[k].c].p.z;

			Tdata[i + 12] = (float)N[T[k].c].p.x;
			Tdata[i + 13] = (float)N[T[k].c].p.y;
			Tdata[i + 14] = (float)N[T[k].c].p.z;
			Tdata[i + 15] = (float)N[T[k].a].p.x;
			Tdata[i + 16] = (float)N[T[k].a].p.y;
			Tdata[i + 17] = (float)N[T[k].a].p.z;
		}

		for (size_t i = 0; i < 4 * number_of_nodes(); i += 4)
		{
			k = i / 4;

			Ndata[i + 0] = N[k].p.x;
			Ndata[i + 1] = N[k].p.y;
			Ndata[i + 2] = N[k].p.z;

			switch (N[k].location)
			{
			case node_location::boundary:
			{
				Ndata[i + 3] = 1.0;
			}
			break;
			case node_location::hole:
			{
				Ndata[i + 3] = 2.0;
			}
			break;
			case node_location::inside:
			{
				Ndata[i + 3] = 3.0;
			}
			break;
			case node_location::outside:
			{
				Ndata[i + 3] = 4.0;
			}
			break;
			default:
				break;
			}
		}

		GLuint bufmesh, bufnode;
		unsigned int vao[2], test;

		glGenVertexArrays(2, vao);

		glBindVertexArray(vao[0]);
		glGenBuffers(1, &bufmesh);
		glBindBuffer(GL_ARRAY_BUFFER, bufmesh);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * number_of_triangles(), Tdata, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

		glBindVertexArray(vao[1]);
		glGenBuffers(1, &bufnode);
		glBindBuffer(GL_ARRAY_BUFFER, bufnode);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * number_of_nodes(), Ndata, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));

		glBindVertexArray(0);

		shadersource src = parseshader("src/shaders/basic_display.glsl");
		unsigned int shader = createshader(src.vertex, src.fragment);
		glUseProgram(shader);

		glEnable(GL_PROGRAM_POINT_SIZE);

		{
			glClear(GL_COLOR_BUFFER_BIT);
			glBindVertexArray(vao[1]);
			glDrawArrays(GL_POINTS, 0, sizeof(float) * 4 * number_of_nodes() / (sizeof(float) * 4));

			glBindVertexArray(vao[0]);
			glDrawArrays(GL_LINES, 0, sizeof(float) * 18 * number_of_triangles() / (sizeof(float) * 3));

			glBindVertexArray(0);

			glfwSwapBuffers(window);
		}

		glDeleteProgram(shader);
		delete[] Tdata;
		delete[] Ndata;
	}
}

//updated on 31/8/18
void mesh::inspect()
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

		float *Tdata, *Ndata;
		Tdata = new float[18 * number_of_triangles()];
		Ndata = new float[4 * number_of_nodes()];

		int k = 0;
		for (int i = 0; i < 18 * number_of_triangles(); i += 18)
		{
			k = i / 18;
			Tdata[i + 0] = (float)N[T[k].a].p.x;
			Tdata[i + 1] = (float)N[T[k].a].p.y;
			Tdata[i + 2] = (float)N[T[k].a].p.z;
			Tdata[i + 3] = (float)N[T[k].b].p.x;
			Tdata[i + 4] = (float)N[T[k].b].p.y;
			Tdata[i + 5] = (float)N[T[k].b].p.z;

			Tdata[i + 6] = (float)N[T[k].b].p.x;
			Tdata[i + 7] = (float)N[T[k].b].p.y;
			Tdata[i + 8] = (float)N[T[k].b].p.z;
			Tdata[i + 9] = (float)N[T[k].c].p.x;
			Tdata[i + 10] = (float)N[T[k].c].p.y;
			Tdata[i + 11] = (float)N[T[k].c].p.z;

			Tdata[i + 12] = (float)N[T[k].c].p.x;
			Tdata[i + 13] = (float)N[T[k].c].p.y;
			Tdata[i + 14] = (float)N[T[k].c].p.z;
			Tdata[i + 15] = (float)N[T[k].a].p.x;
			Tdata[i + 16] = (float)N[T[k].a].p.y;
			Tdata[i + 17] = (float)N[T[k].a].p.z;
		}

		for (int i = 0; i < 4 * number_of_nodes(); i += 4)
		{
			k = i / 4;

			Ndata[i] = N[k].p.x;
			Ndata[i + 1] = N[k].p.y;
			Ndata[i + 2] = N[k].p.z;

			switch (N[k].location)
			{
			case node_location::boundary:
			{
				Ndata[i + 3] = 1.0;
			}
			break;
			case node_location::hole:
			{
				Ndata[i + 3] = 2.0;
			}
			break;
			case node_location::inside:
			{
				Ndata[i + 3] = 3.0;
			}
			break;
			case node_location::outside:
			{
				Ndata[i + 3] = 4.0;
			}
			break;
			default:
				break;
			}
		}

		GLuint bufmesh, bufnode;
		unsigned int vao[3], test;

		glGenVertexArrays(3, vao);

		glBindVertexArray(vao[0]);
		glGenBuffers(1, &bufmesh);
		glBindBuffer(GL_ARRAY_BUFFER, bufmesh);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * number_of_triangles(), Tdata, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

		glBindVertexArray(vao[1]);
		glGenBuffers(1, &bufnode);
		glBindBuffer(GL_ARRAY_BUFFER, bufnode);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * number_of_nodes(), Ndata, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));

		glBindVertexArray(0);

		shadersource src = parseshader("src/shaders/inspect_display.glsl");
		unsigned int shader = createshader(src.vertex, src.fragment);
		glUseProgram(shader);

		glEnable(GL_PROGRAM_POINT_SIZE);

		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
		int loc_trans = glGetUniformLocation(shader, "translate");
		glUniformMatrix4fv(loc_trans, 1, GL_FALSE, &trans[0][0]);

		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
		int loc_sc = glGetUniformLocation(shader, "scale");
		glUniformMatrix4fv(loc_sc, 1, GL_FALSE, &scale[0][0]);

		float psize = zoom;
		int loc_psize = glGetUniformLocation(shader, "psize");
		glUniform1f(loc_psize, psize);

		while (!return_to_console)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glBindVertexArray(vao[1]);
			glDrawArrays(GL_POINTS, 0, sizeof(float) * 4 * number_of_nodes() / (sizeof(float) * 4));

			glBindVertexArray(vao[0]);
			glDrawArrays(GL_LINES, 0, sizeof(float) * 18 * number_of_triangles() / (sizeof(float) * 3));

			trans = glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z));
			glUniformMatrix4fv(loc_trans, 1, GL_FALSE, &trans[0][0]);

			scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
			glUniformMatrix4fv(loc_sc, 1, GL_FALSE, &scale[0][0]);

			psize = zoom;
			glUniform1f(loc_psize, psize);

			glBindVertexArray(0);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteProgram(shader);
		delete[] Tdata;
		delete[] Ndata;
	}

	/*p = { 0,0,0 };
	dx = 0.1;
	dy = 0.1;
	dz = 0.1;
	zoom = 1.0;
	zoom_change = 0.1;
	return_to_console = false;*/
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
	for (size_t i = 1; i < number_of_triangles(); i++)
	{
		res += triangle_area(T[i]);
	}
	return res / (number_of_triangles() * 1.0);
}

//updated on 23/8/18
double mesh::avg_area_of_triangles_near_boundary(node_location loc)
{
	double res = 0;
	int count = 0;
	for (size_t i = 1; i < number_of_triangles(); i++)
	{
		if (N[T[i].a].location == loc || N[T[i].b].location == loc || N[T[i].c].location == loc)
		{
			res += triangle_area(T[i]);
			++count;
		}
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
		N[m_e.start].IE.push_back(m_e.id);
		N[m_e.end].T.push_back(m_e.id);
	}
}

void mesh::edge_triangle_share_sweep()
{
	for (edge &m_e : E)
	{
		uint64_t count = 0;
		unordered_map<uint64_t, bool> triangle_track;
		for (const uint64_t i : N[m_e.start].T)
			triangle_track[i] = true;

		for (const uint64_t i : N[m_e.end].T)
		{
			if (triangle_track.find(i) != triangle_track.end())
			{
				m_e.T.push_back(i);
				++count;
			}

			if(count ==2 && m_e.location == edge_location::inside) break;
			else if(count==1 && m_e.location == edge_location::boundary) break;
		}
	}
}

void mesh::triangle_edge_share_sweep()
{
	for(const edge& m_e : E)
	{
		for(const uint64_t t_id : m_e.T)
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
pair<node_id,node_id> mesh::corner_pos(const node &n)
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
pos mesh::generate_ghost_point(const triangle_id t_id , const node_id n_id)
{
	pos res;
	pos p = N[n_id].p;
	double angle;
	if (T[t_id].a == n_id)
	{
		angle = line_inclination_absolute({N[T[t_id].b].p, N[T[t_id].c].p});
		p = p - N[T[t_id].b].p;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res = res + N[T[t_id].b].p;
	}

	else if (T[t_id].b == n_id)
	{
		angle = line_inclination_absolute({N[T[t_id].c].p, N[T[t_id].a].p});
		p = p - N[T[t_id].c].p;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res = res + N[T[t_id].c].p;
	}

	else //if (T[t_id].c == n_id)
	{
		angle = line_inclination_absolute({N[T[t_id].a].p, N[T[t_id].b].p});
		p = p - N[T[t_id].a].p;
		p = rotate_point(p, (2 * pi - angle));
		res.x = p.x;
		res.y = -p.y;
		res = rotate_point(res, angle);
		res = res + N[T[t_id].a].p;
	}

	return res;
}

pos mesh::generate_centroid(const std::vector<node> &m_N)
{
	pos centroid;
	for (const node &n : m_N)
		centroid = centroid + n.p;

	return centroid / m_N.size();
}

/*
//updated on 23/8/18
//Checks whether a side of the triangle consists of only on edge element in the boundary
//This function is only used by generate_ghosts
bool connected_node(node *a, node *b)
{
	if (a == a->BE[0]->start && b == a->BE[0]->end)
		return true;

	else if (a == a->BE[0]->end && b == a->BE[0]->start)
		return true;

	if (a == a->BE[1]->start && b == a->BE[1]->end)
		return true;

	else if (a == a->BE[1]->end && b == a->BE[1]->start)
		return true;

	else
		return false;
}
*/
/*
! Need to update edge swap
//updated on 23/8/18
//Finds the common edge between two triangles if it exists else returns a edge with zero length
//This function is only used by edge_swap
edge find_common_edge(mesh_triangle *t1, mesh_triangle *t2)
{

	edge e;
	if (((t1->a->p == t2->a->p) || (t1->a->p == t2->b->p) || (t1->a->p == t2->c->p)) && ((t1->b->p == t2->a->p) || (t1->b->p == t2->b->p) || (t1->b->p == t2->c->p)))
	{
		e.start = t1->a;
		e.end = t1->b;
		return e;
	}
	else if (((t1->b->p == t2->a->p) || (t1->b->p == t2->b->p) || (t1->b->p == t2->c->p)) && ((t1->c->p == t2->a->p) || (t1->c->p == t2->b->p) || (t1->c->p == t2->c->p)))
	{
		e.start = t1->b;
		e.end = t1->c;
		return e;
	}
	else if (((t1->c->p == t2->a->p) || (t1->c->p == t2->b->p) || (t1->c->p == t2->c->p)) && ((t1->a->p == t2->a->p) || (t1->a->p == t2->b->p) || (t1->a->p == t2->c->p)))
	{
		e.start = t1->c;
		e.end = t1->a;
		return e;
	}
	else
	{
		node a, b;
		a = {};
		b = {};
		e.start = &a;
		e.end = &b;
		return e;
	}
}
*/
//updated on 23/8/18
//Finds the vertex opposite to the common edge
//This function is only used by edge_swap
const node_id mesh::vertex_opposite_to_triangle_edge(const mesh_triangle& t,const edge & e)
{
	if (N[t.a].id != e.start && N[t.a].id != e.end)
		return t.a;
	else if (N[t.b].id != e.start && N[t.b].id != e.end)
		return t.b;
	else //if (N[T[t_id].c].id != e.start && N[T[t_id].c].id != e.end)
		return t.c;
}


/*
? Not used anywhere currently
int find_triangle_containing_edge(edge &e)
{
	int id = -1;
	for (int i = 0; i < e.start->triangle_share(); i++)
		for (int j = 0; j < e.end->triangle_share(); j++)
		{
			if (e.start->T[i]->id == e.end->T[j]->id)
			{
				id = e.end->T[j]->id;
			}
		}
	return id;
}
*/

void mesh::triangle_node_change(const triangle_id t_id, const node_id fn_id, const node_id tn_id)
{

	//N[fn_id].T.erase(remove(N[fn_id].T.begin(), N[fn_id].T.end(), t_id), N[fn_id].T.end());

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