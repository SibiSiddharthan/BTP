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
	max_edges = 2000;
	max_nodes = 10000;
	max_triangles = 10000;

	N = new node[max_nodes];
	E = new edge[max_edges];
	T = new mesh_triangle[max_triangles];
}

//updated on 29/8/18
void import_2d(mesh &M, _2D_ &_2d_)
{
	M.init_2d();

	for (int i = 0; i < _2d_.number_of_nodes; i++)
	{
		M.N[i] = {_2d_.N[i].p, _2d_.N[i].location, _2d_.N[i].availability};
	}

	for (int i = 0; i < _2d_.number_of_edges; i++)
	{
		M.E[i] = _2d_.E[i];
	}

	M.number_of_nodes = _2d_.number_of_nodes;
	M.number_of_edges = _2d_.number_of_edges;

	for (int i = 0; i < _2d_.number_of_edges; i++)
	{
		if (M.E[i].start->p == M.N[i].p && M.E[i].end->p == M.N[i + 1].p)
		{
			M.E[i].start = &M.N[i];
			M.E[i].end = &M.N[i + 1];
		}

		else if (M.E[i].start->p == M.N[i].p && M.E[i].end->p != M.N[i + 1].p)
		{
			M.E[i].start = &M.N[i];

			for (int j = 0; j < M.number_of_nodes; j++)
			{

				if (M.E[i].end->p == M.N[j].p)
				{

					M.E[i].end = &M.N[j];
					break;
				}
			}
		}
	}

	/*for (int i = 0; i < M.number_of_nodes; i++)
	{
		cout << i << " " << M.N[i].BE.size() << endl;
	}*/

	for (int i = 0; i < M.number_of_edges; i++)
	{
		M.E[i].start->BE.push_back(&M.E[i]);
		M.E[i].end->BE.push_back(&M.E[i]);
	}

	/*for (int i = 0; i < M.number_of_nodes; i++)
	{
		cout << i << " " << M.N[i].BE.size() << endl;
	}*/

	/*for (int i = 0; i < _2d_.number_of_nodes; i++)
	{
		cout << i << " " << _2d_.N[i].BE.size() << endl;
	}*/
}

//updated on 29/8/18
//Displays the number of nodes,number of triangles and the average area of the triangles in the mesh
void mesh::stats()
{
	cout << "Number of nodes: " << number_of_nodes << endl;
	cout << "Number of triangles: " << number_of_triangles << endl;
	cout << "Average area of triangles: " << avg_area_of_triangles() << endl;

	/*cout << "Number of edges: " << number_of_edges << endl;
	int count = 0;
	for (int i = 0; i < number_of_edges; i++)
		if (E[i].location == edge_location::boundary)
			++count;
	cout << "Number of edges on boundary: " << count << endl;*/
}

//updated on 23/8/18

void mesh::generate_mesh_basic()
{
	edge e1, e2, *e3;
	double min_dist = 1.0e10;
	double temp;
	int k = -1;
	while (number_of_unused_edges(E, number_of_edges) != 0)
	{
		for (int i = 0; i < number_of_edges; i++)
		{
			k = -1;
			min_dist = 1.0e10;
			for (int j = 0; j < number_of_nodes; j++)
			{
				if (E[i].availability && N[j].availability && (E[i].start->p != N[j].p && E[i].end->p != N[j].p))
				{

					if (left_test_2d(E[i], N[j]) && (!collinear_test(E[i], N[j])))
					{

						e1.start = &N[j];
						e1.end = E[i].end;
						e2.start = E[i].start;
						e2.end = &N[j];

						if (intersection_test(e1, E, number_of_edges) && intersection_test(e2, E, number_of_edges) /*&& !enclosed_node(E[i].start->p, E[i].end->p, N[j].p, N, number_of_nodes)
							&& !edge_contains_other_nodes(e1, N, number_of_nodes) && !edge_contains_other_nodes(e2, N, number_of_nodes)*/
						)
						{
							temp = distance((E[i].start->p + E[i].end->p) * 0.5, N[j].p);
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

				e1.start = &N[k];
				e1.end = E[i].end;
				e2.start = E[i].start;
				e2.end = &N[k];

				e3 = edge_exists(E, number_of_edges, e1);
				if (e3 != 0)
				{
					e3->availability = false;
					e1 = *e3;
					disable_common_node(&e1, &E[i]);
				}

				else
				{
					E[number_of_edges] = e1;
					E[number_of_edges].availability = true;
					E[number_of_edges].location = edge_location::inside;
					++number_of_edges;
				}

				e3 = edge_exists(E, number_of_edges, e2);
				if (e3 != 0)
				{
					e3->availability = false;
					e2 = *e3;
					disable_common_node(&e2, &E[i]);
				}
				else
				{
					E[number_of_edges] = e2;
					E[number_of_edges].availability = true;
					E[number_of_edges].location = edge_location::inside;
					++number_of_edges;
				}

				make_triangle(E[i].start, E[i].end, &N[k]);
				E[i].availability = false;
				//cout << area_of_triangle(T[nm - 1]) << endl;
				//return;
			}
		}
	}
}

//updated on 23/8/18
void mesh::node_insertion()
{
	pos p0, *p;
	pair<node *, node *> np;

	for (int i = 0; i < number_of_nodes; i++)
	{

		if (N[i].location == node_location::boundary || N[i].location == node_location::hole)
		{
			if (N[i].share > 3)
			{
				p = new pos[N[i].share + 2];
				generate_unique_pos(N[i], p);
				p0 = generate_centroid_for_polygon(p, N[i].share + 2);
				np = corner_pos(N[i]);

				if (distance((np.first->p + np.second->p) * 0.5, N[i].p) < distance((np.first->p + np.second->p) * 0.5, p0))
				{
					N[number_of_nodes].p = p0;
					N[number_of_nodes].location = node_location::inside;
					N[number_of_nodes].availability = false;
					++number_of_nodes;

					for (int j = 0; j < N[i].share; j++)
					{
						N[i].T[j]->node_change(&N[i], &N[number_of_nodes - 1]);
						j--;
					}

					make_triangle(&N[number_of_nodes - 1], &N[i], np.first);
					make_triangle(&N[number_of_nodes - 1], &N[i], np.second);
				}
				delete[] p;
			}
		}
	}
}

//updated on 23/8/18
void mesh::refine_triangles()
{
	double avg = avg_area_of_triangles();
	mesh_triangle *temp;
	temp = new mesh_triangle[3 * number_of_triangles];
	int count = 0;
	int flag = 0;
	if (avg > area_threshold)
	{
		do
		{
			flag = 0;
			count = 0;
			for (int i = 0; i < number_of_triangles; i++)
			{
				if (T[i].area() > 2 * avg && T[i].area() > 2.5 * area_threshold)
				{

					N[number_of_nodes].p = (T[i].a->p + T[i].b->p + T[i].c->p) / 3.0;
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::inside;
					number_of_nodes++;

					temp[count].a = &N[number_of_nodes - 1];
					temp[count].b = T[i].b;
					temp[count].c = T[i].c;
					temp[count].id = count;
					count++;

					temp[count].b = &N[number_of_nodes - 1];
					temp[count].a = T[i].a;
					temp[count].c = T[i].c;
					temp[count].id = count;
					count++;

					temp[count].c = &N[number_of_nodes - 1];
					temp[count].b = T[i].b;
					temp[count].a = T[i].a;
					temp[count].id = count;
					count++;
				}
				else
				{
					temp[count] = T[i];
					temp[count].id = count;
					count++;
				}
			}
			for (int i = 0; i < count; i++)
				T[i] = temp[i];

			number_of_triangles = count;

			for (int i = 0; i < number_of_triangles; i++)
			{
				if (T[i].area() > (2 * avg))
					++flag;
			}

		} while (flag != 0);
		delete[] temp;
	}
	node_triangle_share_sweep();
}

//updated on 23/8/18
void mesh::refine_triangles_near_boundary(node_location _location)
{

	double avg = avg_area_of_triangles_near_boundary(_location);
	mesh_triangle *temp;
	temp = new mesh_triangle[3 * number_of_triangles];
	int count = 0;

	if (avg > area_threshold)
	{
		for (int i = 0; i < number_of_triangles; i++)
		{
			if (T[i].a->location == _location || T[i].b->location == _location || T[i].c->location == _location)
			{
				if (T[i].area() > 1.5 * avg && T[i].area() > 2 * area_threshold)
				{

					N[number_of_nodes].p = (T[i].a->p + T[i].b->p + T[i].c->p) / 3;
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::inside;
					number_of_nodes++;

					temp[count].a = &N[number_of_nodes - 1];
					temp[count].b = T[i].b;
					temp[count].c = T[i].c;
					temp[count].id = count;
					count++;

					temp[count].b = &N[number_of_nodes - 1];
					temp[count].a = T[i].a;
					temp[count].c = T[i].c;
					temp[count].id = count;
					count++;

					temp[count].c = &N[number_of_nodes - 1];
					temp[count].b = T[i].b;
					temp[count].a = T[i].a;
					temp[count].id = count;
					count++;
				}

				else
				{
					temp[count] = T[i];
					temp[count].id = count;
					count++;
				}
			}

			else
			{
				temp[count] = T[i];
				temp[count].id = count;
				count++;
			}
		}

		for (int i = 0; i < count; i++)
			T[i] = temp[i];

		number_of_triangles = count;

		delete[] temp;
	}
	node_triangle_share_sweep();
}

//updated on 23/8/18
void mesh::edge_swap()
{
	edge e;
	node *a, *b;
	mesh_triangle m;
	int id1, id2;
	int flag = 0;
	double tmp1, tmp2, tmp3, tmp4;
	double min_old_1, min_old_2, min_new_1, min_new_2;
	do
	{
		flag = 0;
		for (int k = 0; k < number_of_nodes; k++)
		{
			if (N[k].location == node_location::inside)
			{
				for (int i = 0; i < N[k].share; i++)
				{
					//cout << "i " << i << endl;
					for (int j = 0; j < N[k].share; j++)
					{
						if (i != j)
						{

							e = find_common_edge(N[k].T[i], N[k].T[j]);
							if (e.length() != 0)
							{

								min_old_1 = N[k].T[i]->min_angle();
								min_old_2 = N[k].T[j]->min_angle();

								a = vertex_opposite_to_triangle_edge(N[k].T[i], e);
								b = vertex_opposite_to_triangle_edge(N[k].T[j], e);

								min_new_1 = min_angle_of_triangle(a->p, b->p, e.start->p);
								min_new_2 = min_angle_of_triangle(a->p, b->p, e.end->p);

								tmp1 = area_of_triangle(a->p, b->p, e.start->p);
								tmp2 = area_of_triangle(a->p, b->p, e.end->p);
								tmp3 = N[k].T[i]->area();
								tmp4 = N[k].T[j]->area();

								if (min(min_old_1, min_old_2) < min(min_new_1, min_new_2) && tmp1 > epsilon && tmp2 > epsilon &&
									fabs(tmp3 + tmp4 - (tmp1 + tmp2)) < epsilon)
								{

									++flag;
									id1 = N[k].T[i]->id;
									id2 = N[k].T[j]->id;

									replace_triangle(id1, a, b, e.start);
									replace_triangle(id2, a, b, e.end);

									/*triangle_delete(T, id1);
									triangle_delete(T, id2);
									//cout << "share " << N[k].share << endl;
									put_triangle(T, a, b, e.start, id1);
									put_triangle(T, a, b, e.end, id2);*/

									i--;

									break;
								}
							}
						}
					}
				}
			}
		}

	} while (flag != 0);
}

//updated on 23/8/18
void mesh::centroid_shift()
{
	pos p0, *p;

	for (int i = 0; i < number_of_nodes; i++)
	{
		if (N[i].location == node_location::inside)
		{
			p = new pos[N[i].share + 1];
			generate_unique_pos(N[i], p);
			p0 = generate_centroid_for_polygon(p, N[i].share + 1);
			N[i].p = p0;
			delete[] p;
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
		int n = number_of_triangles;
		for (int i = 0; i < n; i++)
		{
			if ((T[i].a->location == node_location::boundary && T[i].b->location == node_location::boundary && T[i].c->location == node_location::boundary))
			{
				if (T[i].a->share == 1)
				{
					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].c->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;

					make_triangle(T[i].a, T[i].b, &N[number_of_nodes - 1], triangle_type::ghost);

					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].b->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;

					make_triangle(T[i].a, T[i].c, &N[number_of_nodes - 1], triangle_type::ghost);
				}

				else if (T[i].b->share == 1)
				{
					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].c->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;

					make_triangle(T[i].a, T[i].b, &N[number_of_nodes - 1], triangle_type::ghost);

					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].a->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;

					make_triangle(T[i].c, T[i].b, &N[number_of_nodes - 1], triangle_type::ghost);
				}

				else if (T[i].c->share == 1)
				{
					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].a->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;

					make_triangle(T[i].c, T[i].b, &N[number_of_nodes - 1], triangle_type::ghost);

					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].b->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;

					make_triangle(T[i].a, T[i].c, &N[number_of_nodes - 1], triangle_type::ghost);
				}
			}

			else if ((T[i].a->location == node_location::boundary && T[i].b->location == node_location::boundary) ||
					 (T[i].a->location == node_location::hole && T[i].b->location == node_location::hole))
			{

				if (connected_node(T[i].a, T[i].b))
				{
					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].c->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;

					make_triangle(T[i].a, T[i].b, &N[number_of_nodes - 1], triangle_type::ghost);
				}
			}

			else if ((T[i].b->location == node_location::boundary && T[i].c->location == node_location::boundary) ||
					 (T[i].b->location == node_location::hole && T[i].c->location == node_location::hole))
			{

				if (connected_node(T[i].b, T[i].c))
				{
					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].a->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;
					make_triangle(&N[number_of_nodes - 1], T[i].b, T[i].c, triangle_type::ghost);
				}
			}

			else if ((T[i].a->location == node_location::boundary && T[i].c->location == node_location::boundary) ||
					 (T[i].a->location == node_location::hole && T[i].c->location == node_location::hole))
			{

				if (connected_node(T[i].a, T[i].c))
				{
					N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].b->p);
					N[number_of_nodes].availability = false;
					N[number_of_nodes].location = node_location::outside;
					number_of_nodes++;
					make_triangle(T[i].a, &N[number_of_nodes - 1], T[i].c, triangle_type::ghost);
				}
			}
		}
	}
	ghost_generated = true;
}

//Not working
void mesh::generate_ghosts_new()
{
	if (ghost_generated == false)
	{
		int id;
		int count = 0;
		for (int i = 0; i < number_of_edges; i++)
		{
			if (E[i].location == edge_location::boundary)
			{

				id = find_triangle_containing_edge(E[i]);
				if (id != -1)
				{
					++count;
					if (T[id].a->p != E[i].start->p && T[id].a->p != E[i].end->p)
					{
						N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].a->p);
						N[number_of_nodes].availability = false;
						N[number_of_nodes].location = node_location::outside;
						number_of_nodes++;

						make_triangle(T[i].c, T[i].b, &N[number_of_nodes - 1], triangle_type::ghost);
					}

					else if (T[id].b->p != E[i].start->p && T[id].b->p != E[i].end->p)
					{
						N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].b->p);
						N[number_of_nodes].availability = false;
						N[number_of_nodes].location = node_location::outside;
						number_of_nodes++;

						make_triangle(T[i].c, T[i].a, &N[number_of_nodes - 1], triangle_type::ghost);
					}

					else if (T[id].c->p != E[i].start->p && T[id].c->p != E[i].end->p)
					{
						N[number_of_nodes].p = generate_ghost_point({T[i].a->p, T[i].b->p, T[i].c->p}, T[i].c->p);
						N[number_of_nodes].availability = false;
						N[number_of_nodes].location = node_location::outside;
						number_of_nodes++;

						make_triangle(T[i].a, T[i].b, &N[number_of_nodes - 1], triangle_type::ghost);
					}
				}
			}
			//cout << id << endl;
		}
		cout << count;
	}

	ghost_generated = true;
}

//updated on 23/8/18
void mesh::imp_display()
{
	float *Edata, *Ndata;
	Edata = new float[6 * number_of_edges];
	Ndata = new float[4 * number_of_nodes];

	int k = 0;
	for (int i = 0; i < 6 * number_of_edges; i += 6)
	{
		k = i / 6;
		Edata[i] = (float)E[k].start->p.x;
		Edata[i + 1] = (float)E[k].start->p.y;
		Edata[i + 2] = (float)E[k].start->p.z;
		Edata[i + 3] = (float)E[k].end->p.x;
		Edata[i + 4] = (float)E[k].end->p.y;
		Edata[i + 5] = (float)E[k].end->p.z;
	}

	for (int i = 0; i < 4 * number_of_nodes; i += 4)
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * number_of_edges, Edata, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	glBindVertexArray(vao[1]);
	glGenBuffers(1, &bufnode);
	glBindBuffer(GL_ARRAY_BUFFER, bufnode);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * number_of_nodes, Ndata, GL_STATIC_DRAW);
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
		glDrawArrays(GL_POINTS, 0, sizeof(float) * 4 * number_of_nodes / (sizeof(float) * 4));

		glBindVertexArray(vao[0]);
		glDrawArrays(GL_LINES, 0, sizeof(float) * 6 * number_of_edges / (sizeof(float) * 2));

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
		Tdata = new float[18 * number_of_triangles];
		Ndata = new float[4 * number_of_nodes];

		int k = 0;
		for (int i = 0; i < 18 * number_of_triangles; i += 18)
		{
			k = i / 18;
			Tdata[i] = (float)T[k].a->p.x;
			Tdata[i + 1] = (float)T[k].a->p.y;
			Tdata[i + 2] = (float)T[k].a->p.z;
			Tdata[i + 3] = (float)T[k].b->p.x;
			Tdata[i + 4] = (float)T[k].b->p.y;
			Tdata[i + 5] = (float)T[k].b->p.z;

			Tdata[i + 6] = (float)T[k].b->p.x;
			Tdata[i + 7] = (float)T[k].b->p.y;
			Tdata[i + 8] = (float)T[k].b->p.z;
			Tdata[i + 9] = (float)T[k].c->p.x;
			Tdata[i + 10] = (float)T[k].c->p.y;
			Tdata[i + 11] = (float)T[k].c->p.z;

			Tdata[i + 12] = (float)T[k].c->p.x;
			Tdata[i + 13] = (float)T[k].c->p.y;
			Tdata[i + 14] = (float)T[k].c->p.z;
			Tdata[i + 15] = (float)T[k].a->p.x;
			Tdata[i + 16] = (float)T[k].a->p.y;
			Tdata[i + 17] = (float)T[k].a->p.z;
		}

		for (int i = 0; i < 4 * number_of_nodes; i += 4)
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
		unsigned int vao[2], test;

		glGenVertexArrays(2, vao);

		glBindVertexArray(vao[0]);
		glGenBuffers(1, &bufmesh);
		glBindBuffer(GL_ARRAY_BUFFER, bufmesh);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * number_of_triangles, Tdata, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

		glBindVertexArray(vao[1]);
		glGenBuffers(1, &bufnode);
		glBindBuffer(GL_ARRAY_BUFFER, bufnode);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * number_of_nodes, Ndata, GL_STATIC_DRAW);
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
			glDrawArrays(GL_POINTS, 0, sizeof(float) * 4 * number_of_nodes / (sizeof(float) * 4));

			glBindVertexArray(vao[0]);
			glDrawArrays(GL_LINES, 0, sizeof(float) * 18 * number_of_triangles / (sizeof(float) * 3));

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
		Tdata = new float[18 * number_of_triangles];
		Ndata = new float[4 * number_of_nodes];

		int k = 0;
		for (int i = 0; i < 18 * number_of_triangles; i += 18)
		{
			k = i / 18;
			Tdata[i] = (float)T[k].a->p.x;
			Tdata[i + 1] = (float)T[k].a->p.y;
			Tdata[i + 2] = (float)T[k].a->p.z;
			Tdata[i + 3] = (float)T[k].b->p.x;
			Tdata[i + 4] = (float)T[k].b->p.y;
			Tdata[i + 5] = (float)T[k].b->p.z;

			Tdata[i + 6] = (float)T[k].b->p.x;
			Tdata[i + 7] = (float)T[k].b->p.y;
			Tdata[i + 8] = (float)T[k].b->p.z;
			Tdata[i + 9] = (float)T[k].c->p.x;
			Tdata[i + 10] = (float)T[k].c->p.y;
			Tdata[i + 11] = (float)T[k].c->p.z;

			Tdata[i + 12] = (float)T[k].c->p.x;
			Tdata[i + 13] = (float)T[k].c->p.y;
			Tdata[i + 14] = (float)T[k].c->p.z;
			Tdata[i + 15] = (float)T[k].a->p.x;
			Tdata[i + 16] = (float)T[k].a->p.y;
			Tdata[i + 17] = (float)T[k].a->p.z;
		}

		for (int i = 0; i < 4 * number_of_nodes; i += 4)
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * number_of_triangles, Tdata, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

		glBindVertexArray(vao[1]);
		glGenBuffers(1, &bufnode);
		glBindBuffer(GL_ARRAY_BUFFER, bufnode);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * number_of_nodes, Ndata, GL_STATIC_DRAW);
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
			glDrawArrays(GL_POINTS, 0, sizeof(float) * 4 * number_of_nodes / (sizeof(float) * 4));

			glBindVertexArray(vao[0]);
			glDrawArrays(GL_LINES, 0, sizeof(float) * 18 * number_of_triangles / (sizeof(float) * 3));

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

/*Creates a trinagle and updates the shares of it's nodes
a,b,c are node pointers that form the triangle
n is the number of */
//updated on 23/8/18
void mesh::make_triangle(node *a, node *b, node *c, triangle_type tp)
{
	T[number_of_triangles].a = a;
	T[number_of_triangles].b = b;
	T[number_of_triangles].c = c;

	T[number_of_triangles].a->share++;
	T[number_of_triangles].b->share++;
	T[number_of_triangles].c->share++;

	T[number_of_triangles].a->T.push_back(&T[number_of_triangles]);
	T[number_of_triangles].b->T.push_back(&T[number_of_triangles]);
	T[number_of_triangles].c->T.push_back(&T[number_of_triangles]);

	T[number_of_triangles].id = number_of_triangles;
	T[number_of_triangles].type = tp;
	++number_of_triangles;
}

//updated on 23/8/18
void mesh::replace_triangle(int id, node *a, node *b, node *c)
{
	for (int i = 0; i < T[id].a->share; i++)
	{
		if (T + id == T[id].a->T[i])
		{
			T[id].a->T.erase(T[id].a->T.begin() + i);
			T[id].a->share--;
			break;
		}
	}

	for (int i = 0; i < T[id].b->share; i++)
	{
		if (T + id == T[id].b->T[i])
		{
			T[id].b->T.erase(T[id].b->T.begin() + i);
			T[id].b->share--;
			break;
		}
	}

	for (int i = 0; i < T[id].c->share; i++)
	{
		if (T + id == T[id].c->T[i])
		{
			T[id].c->T.erase(T[id].c->T.begin() + i);
			T[id].c->share--;
			break;
		}
	}

	T[id].a = a;
	T[id].b = b;
	T[id].c = c;

	T[id].a->share++;
	T[id].b->share++;
	T[id].c->share++;

	T[id].a->T.push_back(&T[id]);
	T[id].b->T.push_back(&T[id]);
	T[id].c->T.push_back(&T[id]);

	T[id].id = id;
	T[id].type = triangle_type::domain;
}

//updated on 23/8/18
double mesh::avg_area_of_triangles()
{
	double res = 0;
	for (int i = 1; i < number_of_triangles; i++)
	{
		res += T[i].area();
	}
	return res / (number_of_triangles * 1.0);
}

//updated on 23/8/18
double mesh::avg_area_of_triangles_near_boundary(node_location loc)
{
	double res = 0;
	int count = 0;
	for (int i = 1; i < number_of_triangles; i++)
	{
		if (T[i].a->location == loc || T[i].b->location == loc || T[i].c->location == loc)
		{
			res += T[i].area();
			++count;
		}
	}
	return res / count;
}

//updated on 23/8/18
void mesh::node_triangle_share_sweep()
{
	for (int i = 0; i < number_of_nodes; i++)
	{
		N[i].share = 0;
		N[i].T.clear();
		//N[i].m.shrink_to_fit();
	}

	for (int i = 0; i < number_of_triangles; i++)
	{
		T[i].a->T.push_back(&T[i]);
		T[i].a->share++;

		T[i].b->T.push_back(&T[i]);
		T[i].b->share++;

		T[i].c->T.push_back(&T[i]);
		T[i].c->share++;
	}
}

//Not Used
void mesh::input()
{
	cout << "Intructions\n";
	cout << "a:auto mesh generation\n";
	cout << "m:manual mesh generation";
	int gen_mesh = -1;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		gen_mesh = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		gen_mesh = 0;
	}

	if (gen_mesh == 1)
	{
		generate_mesh_full();
		display();
	}
}