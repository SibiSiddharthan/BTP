#include "model2d.h"
#include "dxfreader.h"
#include "gl_abstraction.h"
#include <fmt/core.h>
using namespace std;

namespace __2d__
{
void model::add_boundary_circle(double r, double dx)
{
	pos temp(r, 0);
	int i = 1;
	const uint64_t n = number_of_nodes();
	const uint64_t e = number_of_edges();

	while (dx * i < 2 * pi)
	{
		N.push_back({temp, N.size(), node_location::boundary, true});
		temp.x = r * cos((i)*dx);
		temp.y = r * sin((i)*dx);
		i++;
	}

	for (size_t j = n; j < number_of_nodes(); j++)
		E.push_back({N[j].id, (j + 1 < number_of_nodes() ? N[j + 1].id : N[n].id), E.size(), edge_location::boundary, true});

	/*
	cout << "no of nodes " << number_of_nodes() << endl
		 << "no of edges " << number_of_edges() << endl;
	*/

	for (size_t i = e; i < number_of_edges(); i++)
	{
		N[E[i].start].BE.push_back(E[i].id);
		N[E[i].end].BE.push_back(E[i].id);
	}
}

void model::add_boundary_square(double a, double dx)
{
	const uint64_t c = (uint64_t)(a / dx);

	const uint64_t n = number_of_nodes();
	const uint64_t e = number_of_edges();
	for (size_t i = 0; i < c; i++)
		N.push_back({{-a * 0.5 + i * dx, -a * 0.5}, N.size(), node_location::boundary, true});

	for (size_t i = 0; i < c; i++)
		N.push_back({{a * 0.5, i * dx - a * 0.5}, N.size(), node_location::boundary, true});

	for (size_t i = 0; i < c; i++)
		N.push_back({{a * 0.5 - i * dx, a * 0.5}, N.size(), node_location::boundary, true});

	for (size_t i = 0; i < c; i++)
		N.push_back({{-a * 0.5, a * 0.5 - i * dx}, N.size(), node_location::boundary, true});

	for (size_t j = n; j < number_of_nodes(); j++)
		E.push_back({N[j].id, (j + 1 < number_of_nodes() ? N[j + 1].id : N[n].id), E.size(), edge_location::boundary, true});

	/*
	cout << "no of nodes " << number_of_nodes() << endl
		 << "no of edges " << number_of_edges() << endl;
	*/
	for (size_t i = e; i < number_of_edges(); i++)
	{
		N[E[i].start].BE.push_back(E[i].id);
		N[E[i].end].BE.push_back(E[i].id);
	}
}

void model::add_hole_circle(pos p, double r, double dx)
{
	pos temp(r + p.x, p.y);
	int i = -1;
	const uint64_t n = number_of_nodes();
	const uint64_t e = number_of_edges();
	while (dx * i > -2 * 3.1415)
	{
		N.push_back({temp, N.size(), node_location::hole, true});
		temp.x = p.x + r * cos(i * dx);
		temp.y = p.y + r * sin(i * dx);
		i--;
	}

	for (size_t j = n; j < number_of_nodes(); j++)
		E.push_back({N[j].id, (j + 1 < number_of_nodes() ? N[j + 1].id : N[n].id), E.size(), edge_location::boundary, true});

	/*
	cout << "no of nodes " << number_of_nodes() << endl
		 << "no of edges " << number_of_edges() << endl;
	*/
	for (size_t i = e; i < number_of_edges(); i++)
	{
		N[E[i].start].BE.push_back(E[i].id);
		N[E[i].end].BE.push_back(E[i].id);
	}
}

void model::add_hole_square(pos p, double a, double dx)
{
	pos lb = {p.x - (a * 0.5), p.y - (a * 0.5)};
	pos lt = {p.x - (a * 0.5), p.y + (a * 0.5)};
	pos rt = {p.x + (a * 0.5), p.y + (a * 0.5)};
	pos rb = {p.x + (a * 0.5), p.y - (a * 0.5)};
	const uint64_t n = number_of_nodes();
	const uint64_t e = number_of_edges();
	const uint64_t c = (uint64_t)(a / dx);
	for (size_t i = 0; i < c; i++)
		N.push_back({{lb.x, lb.y + i * dx}, N.size(), node_location::hole, true});

	for (size_t i = 0; i < c; i++)
		N.push_back({{lt.x + i * dx, lt.y}, N.size(), node_location::hole, true});

	for (size_t i = 0; i < c; i++)
		N.push_back({{rt.x, rt.y - i * dx}, N.size(), node_location::hole, true});

	for (size_t i = 0; i < c; i++)
		N.push_back({{rb.x - i * dx, rb.y}, N.size(), node_location::hole, true});

	for (size_t j = n; j < number_of_nodes(); j++)
		E.push_back({N[j].id, (j + 1 < number_of_nodes() ? N[j + 1].id : N[n].id), E.size(), edge_location::boundary, true});

	/*
	cout << "no of nodes " << number_of_nodes() << endl
		 << "no of edges " << number_of_edges() << endl;
	*/
	for (size_t i = e; i < number_of_edges(); i++)
	{
		N[E[i].start].BE.push_back(E[i].id);
		N[E[i].end].BE.push_back(E[i].id);
	}
}

void model::display()
{
	window w(800, 800);
	vector<float> posdata = export_vertex_data();
	vector<GLuint> node_index = export_node_index();
	vector<GLuint> edge_index = export_edge_index();
	vector<color> node_color(number_of_nodes(), colors("yellow"));
	vector<color> edge_color(number_of_nodes(), colors("red"));
	vector<float> node_size(number_of_nodes(), 5.0);
	ImGuiWindowFlags mf = ImGuiWindowFlags_NoResize;
	pos p;
	float zoom;
	if (number_of_nodes() > 0 && number_of_edges() > 0)
	{
		data_buffer vb_pos(GL_ARRAY_BUFFER, posdata);
		vb_pos.configure_layout(1, 3, 3, GL_FLOAT);

		data_buffer ib_node(GL_ELEMENT_ARRAY_BUFFER, node_index), ib_edge(GL_ELEMENT_ARRAY_BUFFER, edge_index);

		data_buffer cb_node(GL_ARRAY_BUFFER, node_color);
		cb_node.configure_layout(2, 3, 3, GL_FLOAT);

		data_buffer cb_edge(GL_ARRAY_BUFFER, edge_color);
		cb_edge.configure_layout(2, 3, 3, GL_FLOAT);

		data_buffer db_node_size(GL_ARRAY_BUFFER, node_size);
		db_node_size.configure_layout(3, 1, 1, GL_FLOAT);

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
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
		glm::mat4 mvp = trans * scale;
		prog.set_uniform("MVP", uniform_types::MAT4F, (void *)&mvp[0][0]);
		prog.set_uniform("psize", uniform_types::FLOAT, (void *)&zoom);

		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_LINE_SMOOTH);

		ImGuiIO &io = ImGui::GetIO();
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
			scale = glm::scale(glm::mat4(1.0), glm::vec3(zoom));
			mvp = trans * scale;

			prog.update_uniforms();

			ImGui::Begin("Stats", (bool *)false, mf);
			ImGui::Text(stats().c_str());
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			w.swap_buffers();
			w.poll_events();
		}
	}
}

void model::dxf_read(const std::string filepath, double dx)
{
	ifstream f;
	vector<line> L;
	read_file(f, filepath, L, dx);

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
		for (size_t i = 0; i < L.size(); i++)
			N.push_back({L[i].start, N.size(), node_location::boundary, true});

		for (size_t j = 0; j < number_of_nodes(); j++)
		{

			/*
			E[j].start = &N[j];

			if (j + 1 < number_of_nodes)
				E[j].end = &N[j + 1];
			else
				E[j].end = &N[0];

			E[j].availability = true;
			E[j].location = edge_location::boundary;
			*/
		}

		cout << "no of nodes " << number_of_nodes() << endl
			 << "no of edges " << number_of_edges() << endl;
		for (size_t i = 0; i < number_of_edges(); i++)
		{
			N[E[i].start].BE.push_back(E[i].id);
			N[E[i].end].BE.push_back(E[i].id);
		}
	}
}

void model::intersections(vector<line> &l, vector<pair<line, line>> &il)
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

void model::branches_and_gaps(vector<line> &l, vector<pos> &p)
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

vector<float> model::export_vertex_data() const
{
	vector<float> POS(3 * N.size());
	size_t k = 0;
	for (size_t i = 0; i < 3 * N.size(); i += 3)
	{
		k = i / 3;
		POS[i + 0] = float(N[k].p.x);
		POS[i + 1] = float(N[k].p.y);
		POS[i + 2] = float(N[k].p.z);
	}
	return POS;
}

vector<uint32_t> model::export_node_index() const
{
	vector<uint32_t> I(N.size());
	for (size_t i = 0; i < N.size(); ++i)
		I[i] = N[i].id;
	return I;
}

vector<uint32_t> model::export_edge_index() const
{
	vector<uint32_t> I(E.size() * 2);
	size_t k = 0;
	for (size_t i = 0; i < E.size() * 2; i += 2)
	{
		k = i / 2;
		I[i + 0] = uint32_t(E[k].start);
		I[i + 1] = uint32_t(E[k].end);
	}
	return I;
}

string model::stats() const
{
	string stat = fmt::format("Number of nodes:{}\nNumber of edges:{}", number_of_nodes(), number_of_edges());
	return stat;
}

void model::save(const string &filepath)
{
	if (number_of_nodes() != 0 && number_of_edges() != 0)
	{
		FILE *fout = fopen(filepath.c_str(), "wb");
		fprintf(fout, "%llu %llu\n", number_of_nodes(), number_of_edges());
		for (const node &n : N)
			fprintf(fout, "%lf %lf %d\n", n.p.x, n.p.y, (int)n.location);
		for (const edge &e : E)
			fprintf(fout, "%llu %llu %d\n", e.start, e.end, (int)e.location);
		fclose(fout);
	}
	else
		printf("model is empty\n");
}

void model::load(const string &filepath)
{
	if (number_of_nodes() == 0 && number_of_edges() == 0)
	{
		FILE *fin = fopen(filepath.c_str(), "rb");
		if (fin)
		{
			size_t nn, ne;
			fscanf(fin, "%llu %llu\n", &nn, &ne);

			N.reserve(nn);
			E.reserve(ne);

			for (size_t i = 0; i < nn; ++i)
			{
				double x, y;
				int loc;
				fscanf(fin, "%lf %lf %d\n", &x, &y, &loc);
				N.push_back({{x, y, 0.0}, i, static_cast<__2d__::node_location>(loc)});
			}

			for (size_t i = 0; i < ne; ++i)
			{
				size_t start, end;
				int loc;
				fscanf(fin, "%llu %llu %d\n", &start, &end, &loc);
				E.push_back({start, end, i, static_cast<__2d__::edge_location>(loc)});
				N[E[i].start].BE.push_back(E[i].id);
				N[E[i].end].BE.push_back(E[i].id);
			}
		}

		else
			printf("file not found\n");

		fclose(fin);
	}
	else
		printf("model is not empty\n");
}

} // namespace __2d__