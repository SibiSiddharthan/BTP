#include "mesh.h"

using namespace std;

bool quit = false;

void input(_2D_ &boundary, mesh &M)
{

	cout << "Enter input" << endl;
	int a;
	cin >> a;
	{
		auto start = chrono::high_resolution_clock::now();
		switch (a)
		{

		case 0:
		{
			M.stats();
		}
		break;

		case 1:
		{
			M.generate_mesh_basic();
			M.display();
		}
		break;

		case 2:
		{
			M.node_insertion();
			M.display();
		}
		break;

		case 3:
		{
			M.refine_triangles();
			M.display();
		}
		break;

		case 4:
		{
			M.edge_swap();
			M.display();
		}
		break;

		case 5:
		{
			M.refine_triangles_near_boundary(node_location::boundary);
			M.display();
		}
		break;

		case 6:
		{
			M.refine_triangles_near_boundary(node_location::hole);
			M.display();
		}
		break;

		case 7:
		{
			M.centroid_shift();
			M.display();
		}
		break;

		case 8:
		{
			M.generate_mesh_full();
			M.display();
		}
		break;

		case 9:
		{
			M.generate_ghosts();
			M.display();
		}
		break;

		case 10:
		{
			M.display();
		}
		break;

		case 100:
		{
			glfwPostEmptyEvent();
			cout << "Entering inspect mode\n";
			M.display(true);
			cout << "Returning to console\n";
			M.display();
			glfwWaitEventsTimeout(100.0);
		}
		break;

		case 11:
		{
			M.generate_mesh_basic(true);
			M.display();
		}break;

		case 12:
		{
			M.node_insertion(true);
			M.display();
		}
		break;

		case 13:
		{
			M.refine_triangles(true);
			M.display();
		}
		break;

		case 14:
		{
			M.edge_swap(true);
			M.display();
		}
		break;

		case 15:
		{
			M.centroid_shift(true);
			M.display();
		}

		case 999:
		{
			quit = true;
		}
		break;

		default:
			break;
		}

		auto finish = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed_time = finish - start;
		cout << "Time Taken for operation: " << elapsed_time.count() << endl;
	}
}

int main()
{

	GLFWwindow *window = nullptr;
	create_window(window, 800, 800);

	_2D_ boundary;
	mesh M;
	boundary.attach_window(window);
	boundary.add_boundary_circle(0.6, 0.3);
	//boundary.add_hole_square({ 0,0,0 }, 0.4, 0.05);
	//boundary.add_boundary_square( 0.8, 0.05);
	/*boundary.add_hole_circle({ 0.2,0.2 }, 0.1, 0.2 );
	boundary.add_hole_circle({ -0.2,0.2 }, 0.1, 0.2);
	boundary.add_hole_circle({ 0.2,-0.2 }, 0.1, 0.2);
	boundary.add_hole_circle({ -0.2,-0.2 }, 0.1, 0.2);*/
	//boundary.dxf_read("tests/polyline.dxf", 4.0);

	//glfwWaitEventsTimeout(100.0);

	if (boundary.transferable)
	{
		boundary.display();
		import_2d(M, boundary);
		M.attach_window(window);
		
		cout << "0:stats\n1:generate triangles\n2:node insertion\n3:equalize triangles\n4:edge swap\n"
			 << "5:equalize triangles near boundary\n6:equalize triangles near hole\n7:node shift\n8:auto generate\n9:generate ghost triangles\n10:display\n100:inspect\n999:quit\n";
		while (!quit)
		{
			glfwWaitEventsTimeout(100.0);
			input(boundary, M);
			glfwPostEmptyEvent();
		}
	}

	else
	{
		cout << "Errors in the Drawing\n";
		cin.get();
	}

	glfwTerminate();

	cin.get();

	return 0;
}
