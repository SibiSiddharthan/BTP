//#include "stl_reader.h"
//#include "3d.h"

#include "mesh.h"
#include "model.h"
#include "solver.h"

using namespace std;


int main()
{

	//window W(800,800);

	
	//model2d m,x;
	//m.add_boundary_circle(0.6,0.1);
	//m.add_boundary_square(1.0,0.1);
	//m.add_hole_circle({ 0.2,0.2 }, 0.1, 0.2 );
	//m.add_hole_circle({ -0.2,0.2 }, 0.1, 0.2);
	//m.add_hole_circle({ 0.2,-0.2 }, 0.1, 0.2);
	//m.add_hole_circle({ -0.2,-0.2 }, 0.1, 0.2);
	//m.save("2dout.dat");
	//x.load("2dout.dat");
	//x.display();
	//mesh2d g;
	//g.import_2d(m);
	//g.generate_interactive();
	
	//g.generate_mesh_basic();
	//y.load("save1.dat");
	//g.generate_mesh_full();
	//g.display();
	//g.save("save1.dat");
	//g.generate_mesh_basic_debug();
	//g.node_insertion_debug();
	//g.refine_triangles_debug();
	//g.refine_triangles_near_boundary_debug(__2d__::node_location::boundary);
	//g.edge_swap_debug();
	//g.centroid_shift_debug();
	//g.save("./tests/save1.dat");
	//g.load("./tests/save1.dat");
	//g.display();
	
	//solver2d s;
	//s.import_mesh(g);
	//s.solve();
	//s.display();


	
	model3d surf;
	surf.add_boundary_sphere(0.6,0.3);
	//surf.add_hole_sphere({0,0.0,0},0.6,0.1);
	//surf.add_boundary_cube(1.0,0.1);
	//surf.add_hole_cube({0.2,0.1},1.0,0.1);
	//surf.stl_read("tests/sphere0.stl");
	//surf.display();
	mesh3d m;
	m.import_3d(surf);
	//m.generate_mesh_basic_debug();
	m.generate_mesh_basic();
	m.generate_mesh_basic_debug();
	m.display();
	

	return 0;
}
