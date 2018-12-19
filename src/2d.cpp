#include"2d.h"
#include<iostream>
#include"dxfreader.h"

using namespace std;

void _2D_::init()
{
	max_nodes = 2000;
	max_edges = 2000;

	number_of_nodes = 0;
	number_of_edges = 0;

	N = new node[max_nodes];
	E = new edge[max_edges];
}


void _2D_::add_boundary_circle(double r, double dx)
{
	pos temp(r, 0);
	int i = 1, n = number_of_nodes;
	int e = number_of_edges;

	while (dx*i < 2 * pi_)
	{
		N[number_of_nodes] = { temp,node_location::boundary,true };
		temp.x = r * cos((i)*dx);
		temp.y = r * sin((i)*dx);
		i++;
		++number_of_nodes;

	}


	for (int j = n; j < number_of_nodes; j++)
	{
		E[j].start = &N[j];

		if (j + 1 < number_of_nodes)
			E[j].end = &N[j + 1];
		else
			E[j].end = &N[n];

		E[j].availability = true;
		E[j].location = edge_location::boundary;
		++number_of_edges;

	}
	cout << "no of nodes " << number_of_nodes << endl << "no of edges " << number_of_edges << endl;

	for (int i = e; i < number_of_edges; i++)
	{
		E[i].start->BE.push_back(&E[i]);
		E[i].end->BE.push_back(&E[i]);
	}
}

void _2D_::add_boundary_square(double a, double dx)
{
	int c = (int)(a / dx);

	int n = number_of_nodes;
	int e = number_of_edges;
	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = { { -a * 0.5 + i * dx,-a * 0.5 },node_location::boundary,true };
		++number_of_nodes;
		
	}

	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = {{ a*0.5 ,i*dx - a * 0.5 },node_location::boundary,true };
		++number_of_nodes;
	}

	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = {{ a*0.5 - i * dx,a * 0.5 },node_location::boundary,true };
		++number_of_nodes;
	}

	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = {{ -a * 0.5,a*0.5 - i * dx },node_location::boundary,true };
		++number_of_nodes;
	}

	for (int j = n; j < number_of_nodes; j++)
	{
		E[j].start = &N[j];

		if (j + 1 < number_of_nodes)
			E[j].end = &N[j + 1];
		else
			E[j].end = &N[n];

		E[j].availability = true;
		E[j].location = edge_location::boundary;
		++number_of_edges;

	}
	cout << "no of nodes " << number_of_nodes << endl << "no of edges " << number_of_edges << endl;
	for (int i = e; i < number_of_edges; i++)
	{
		E[i].start->BE.push_back(&E[i]);
		E[i].end->BE.push_back(&E[i]);
	}
}

void _2D_::add_hole_circle(pos p, double r, double dx)
{
	pos temp(r + p.x, p.y);
	int i = -1;
	int n = number_of_nodes;
	int e = number_of_edges;
	while (dx*i > -2 * 3.1415)
	{
		N[number_of_nodes] = { temp,node_location::hole,true };
		temp.x = p.x + r * cos(i*dx);
		temp.y = p.y + r * sin(i*dx);
		i--; 
		++number_of_nodes;
	}

	for (int j = n; j < number_of_nodes; j++)
	{
		E[j].start = &N[j];

		if (j + 1 < number_of_nodes)
			E[j].end = &N[j + 1];
		else
			E[j].end = &N[n];

		E[j].availability = true;
		E[j].location = edge_location::boundary;
		++number_of_edges;

	}

	cout << "no of nodes " << number_of_nodes << endl << "no of edges " << number_of_edges << endl;
	for (int i = e; i < number_of_edges; i++)
	{
		E[i].start->BE.push_back(&E[i]);
		E[i].end->BE.push_back(&E[i]);
	}
}

void _2D_::add_hole_square(pos p, double a, double dx)
{
	pos lb = { p.x - (a * 0.5),p.y - (a * 0.5) };
	pos lt = { p.x - (a * 0.5),p.y + (a * 0.5) };
	pos rt = { p.x + (a * 0.5),p.y + (a * 0.5) };
	pos rb = { p.x + (a * 0.5),p.y - (a * 0.5) };
	int n = number_of_nodes;
	int e = number_of_edges;
	int c = (int)(a / dx);
	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = { { lb.x ,lb.y + i * dx },node_location::hole,true };
		++number_of_nodes;
	}

	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = {{ lt.x + i * dx ,lt.y },node_location::hole,true };
		++number_of_nodes;
	}

	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = {{ rt.x,rt.y - i * dx },node_location::hole,true };
		++number_of_nodes;
	}

	for (int i = 0; i < c; i++)
	{
		N[number_of_nodes] = {{ rb.x - i * dx,rb.y },node_location::hole,true };
		++number_of_nodes;
	}

	for (int j = n; j < number_of_nodes; j++)
	{
		E[j].start = &N[j];

		if (j + 1 < number_of_nodes)
			E[j].end = &N[j + 1];
		else
			E[j].end = &N[n];

		E[j].availability = true;
		E[j].location = edge_location::boundary;
		++number_of_edges;

	}

	cout << "no of nodes " << number_of_nodes << endl << "no of edges " << number_of_edges << endl;
	for (int i = e; i < number_of_edges; i++)
	{
		E[i].start->BE.push_back(&E[i]);
		E[i].end->BE.push_back(&E[i]);
	}
}

void _2D_::display()
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
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		

		glBindVertexArray(vao[1]);
		glGenBuffers(1, &bufnode);
		glBindBuffer(GL_ARRAY_BUFFER, bufnode);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * number_of_nodes, Ndata, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

		
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

void _2D_::dxf_read(const std::string filepath,double dx)
{
	ifstream f;
	vector<line> L;
	read_file(f, filepath, L,dx);

	vector<pair<line, line>> intersecting_lines;
	intersections(L, intersecting_lines);

	vector<pos> P;
	branches_and_gaps(L, P);
	//cout << L.size() << " " << P.size() << endl;

	if (intersecting_lines.size() == 0 && P.size() == 0)
		transferable = true;
	else
		transferable = false;

	//Only works if figure is drawn counter-clockwise
	if (transferable)
	{
		for (int i = 0; i < L.size(); i++)
		{
			N[number_of_nodes] = { L[i].start,node_location::boundary,true };
			++number_of_nodes;
		}

		for (int j = 0; j < number_of_nodes; j++)
		{
			E[j].start = &N[j];

			if (j + 1 < number_of_nodes)
				E[j].end = &N[j + 1];
			else
				E[j].end = &N[0];

			E[j].availability = true;
			E[j].location = edge_location::boundary;
			++number_of_edges;

		}

		cout << "no of nodes " << number_of_nodes << endl << "no of edges " << number_of_edges << endl;
		for (int i = 0; i < number_of_edges; i++)
		{
			E[i].start->BE.push_back(&E[i]);
			E[i].end->BE.push_back(&E[i]);
		}
	}

}

void _2D_::intersections(vector<line> &l, vector<pair<line, line>> &il)
{

	for (int i = 0; i < l.size(); i++)
	{
		for (int j = i + 1; j < l.size(); j++)
		{
			if (do_they_intersect(l[i], l[j]))
			{
				il.push_back(make_pair(l[i], l[j]));
			}

		}
	}


}

void _2D_::branches_and_gaps(vector<line> &l, vector<pos> &p)
{
	pos *plist = new pos[2 * l.size()];
	int count = 0;
	int *pcount = new int[2 * l.size()];
	int flag;

	for (int i = 0; i < l.size(); i++)
	{

		if (i != 0)
		{
			//starting pos
			flag = 0;
			for (int j = 0; j < count; j++)
			{
				if (l[i].start == plist[j])
				{
					flag++;
					
				}
			}

			//new unique pos
			if (flag == 0)
			{
				plist[count] = l[i].start;
				
			}

			//ending pos
			flag = 0;
			for (int j = 0; j < count; j++)
			{
				if (l[i].end == plist[i])
				{
					flag++;
					
				}
			}

			//new unique pos
			if (flag == 0)
			{
				plist[count] = l[i].end;
				
			}

		}

		else
		{
			plist[count] = l[i].start;
			plist[count] = l[i].end;
			
		}
		
	}

	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < l.size(); j++)
		{
			if (plist[i] == l[j].start || plist[i] == l[j].end)
				++pcount[i];
		}
	}

	for (int i = 0; i < count; i++)
	{
		//cout << pcount[i] << endl;
		if (pcount[i] != 1)
		{
			p.push_back(plist[i]);
		}
	}

	delete[] plist;
	delete[] pcount;
}