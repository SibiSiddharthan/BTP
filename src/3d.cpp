#include "3d.h"
#include "stl_reader.h"
using namespace std;

void _3D_::stl_read(const string filepath)
{
	ifstream f;
	read_stl(f,"tests/sphere1.stl",P);


}

void _3D_::display()
{
	vector<float> posdata(9 * P.size());
	vector<color> pcolor(3 * P.size(),colors("yellow"));
	vector<GLuint> p_index;

	uint64_t k = 0;
	for(size_t i = 0 ; i< 9 * P.size(); i+=9)
	{
		k = i/9;

		posdata[i + 0] = (float)P[k].a.x;
		posdata[i + 1] = (float)P[k].a.y;
		posdata[i + 2] = (float)P[k].a.z;
		p_index.push_back(p_index.size());

		posdata[i + 3] = (float)P[k].b.x;
		posdata[i + 4] = (float)P[k].b.y;
		posdata[i + 5] = (float)P[k].b.z;
		p_index.push_back(p_index.size());

		posdata[i + 6] = (float)P[k].c.x;
		posdata[i + 7] = (float)P[k].c.y;
		posdata[i + 8] = (float)P[k].c.z;
		p_index.push_back(p_index.size());
/*
		pcolor[i + 0] = (float)P[i].a.x;
		pcolor[i + 1] = (float)P[i].a.y;
		pcolor[i + 2] = (float)P[i].a.z;

		pcolor[i + 3] = (float)P[i].b.x;
		pcolor[i + 4] = (float)P[i].b.y;
		pcolor[i + 5] = (float)P[i].b.z;

		pcolor[i + 6] = (float)P[i].c.x;
		pcolor[i + 7] = (float)P[i].c.y;
		pcolor[i + 8] = (float)P[i].c.z;
*/
	}

	data_buffer vb(GL_ARRAY_BUFFER,posdata);
	vb.configure_layout(1,3,3,GL_FLOAT);

	data_buffer cb(GL_ARRAY_BUFFER,pcolor);
	cb.configure_layout(2,3,3,GL_FLOAT);

	data_buffer ib(GL_ELEMENT_ARRAY_BUFFER,p_index);

	vertex_array va(GL_TRIANGLES);

	va.bind();
	va.bind_buffer({&vb,&cb,&ib});
	va.unbind();

	shadersource src = parseshader("src/shaders/debug_display.glsl");
	unsigned int shader = createshader(src.vertex, src.fragment);
	glUseProgram(shader);

	glClear(GL_COLOR_BUFFER_BIT);
	va.draw();
	glfwSwapBuffers(window);

}