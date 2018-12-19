#shader vertex
#version 430 core

layout(location = 1) in vec4 position;
layout(location = 2) in float type;

out vec4 ncolor;


void main()
{
	gl_Position = position;
	ncolor = vec4(1.0, 0, 0, 1.0);
	gl_PointSize = 4.0;

	if (type == 1.0)
	{
		ncolor = vec4(0, 1.0, 0, 1.0);
		gl_PointSize = 8.0;
	}
	else if (type == 2.0)
	{
		ncolor = vec4(0, 1.0, 0, 1.0);
		gl_PointSize =  4.0;
	}
	else if (type == 3.0)
	{
		ncolor = vec4(1.0, 1.0, 0, 1.0);
		gl_PointSize =  3.0;
	}

	else if (type == 4.0)
	{
		ncolor = vec4(1.0, 0, 1.0, 1.0);
		gl_PointSize =  2.0;
	}

	else if (type == 5.0)
	{
		ncolor = vec4(1.0, 1.0, 0.0, 1.0);
		gl_PointSize =  5.0;
	}

}

#shader fragment
#version 430 core

in vec4 ncolor;
out vec4 colour;

//uniform vec4 color;

void main()
{
	colour = ncolor;
	/*colour = vec4( 1.0, 0.0, 0.0, 0.0 )*/;
}