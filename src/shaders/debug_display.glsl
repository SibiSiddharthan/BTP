#shader vertex
#version 430 core

layout(location = 1) in vec4 position;
layout(location = 2) in vec3 color;
layout(location = 3) in float size;

out vec4 node_color;

void main()
{
	gl_Position = position;
	gl_PointSize = size;
	node_color = vec4(color,1.0);
	
}

#shader fragment
#version 430 core

in vec4 node_color;
out vec4 color;

void main()
{
	color = node_color;
}