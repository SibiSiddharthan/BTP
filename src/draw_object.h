#ifndef DRAW_OBJECT_H
#define DRAW_OBJECT_H

#include "position.h"
#include "gl_abstraction.h"

enum class primitive_types
{
	point,
	line,
	triangle,
	//triangle_loop,
	//tetrahedron_loop
};

struct primitive
{
	std::vector<uint32_t> indexes;
	primitive_types object_type;
	color object_color;

	/*inline void draw_object() const
	{

	}*/
};

class draw_object
{
  private:
	std::vector<float> vertex_data;
	std::vector<primitive> objects;

  public:
	inline void set_vertex_data(const std::vector<float> &_vertex_data)
	{
		vertex_data = _vertex_data;
	}

	inline void set_objects(const primitive& p)
	{
		objects.push_back(p);
	}

	inline void draw_objects() const
	{
		data_buffer vertexes(GL_ARRAY_BUFFER,vertex_data);
		vertexes.configure_layout(1, 3, 3, GL_FLOAT);

		for(const primitive& p:objects)
		{
			std::vector<color> obj_color(vertex_data.size()/3,p.object_color);
			data_buffer color_buffer(GL_ARRAY_BUFFER,obj_color);
			color_buffer.configure_layout(2, 3, 3, GL_FLOAT);
			data_buffer index_buffer(GL_ELEMENT_ARRAY_BUFFER,p.indexes);

			switch(p.object_type)
			{ 
				case primitive_types::point:
				{
					vertex_array va(GL_POINTS);
					va.bind();
					va.bind_buffer({&vertexes,&color_buffer,&index_buffer});
					va.draw();
					
				}break;

				case primitive_types::line:
				{
					vertex_array va(GL_LINES);
					va.bind();
					va.bind_buffer({&vertexes,&color_buffer,&index_buffer});
					va.draw();
					
				}break;

			}
		}
	}
};

#endif