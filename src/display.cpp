#include "display.h"

void keycallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_W:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				p.y -= 0.5 * dy;
			else if (mods == GLFW_MOD_CONTROL)
				p.y -= 2 * dy;
			else
				p.y -= dy;
		}
	}
	break;

	case GLFW_KEY_S:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				p.y += 0.5 * dy;
			else if (mods == GLFW_MOD_CONTROL)
				p.y += 2 * dy;
			else
				p.y += dy;
		}
	}
	break;

	case GLFW_KEY_A:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				p.x += 0.5 * dx;
			else if (mods == GLFW_MOD_CONTROL)
				p.x += 2 * dx;
			else
				p.x += dx;
		}
	}
	break;

	case GLFW_KEY_D:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (mods == GLFW_MOD_SHIFT)
				p.x -= 0.5 * dx;
			else if (mods == GLFW_MOD_CONTROL)
				p.x -= 2 * dx;
			else
				p.x -= dx;
		}
	}
	break;

	case GLFW_KEY_KP_ADD:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			zoom *= zoom_change;
		}
	}
	break;

	case GLFW_KEY_KP_SUBTRACT:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			zoom /= zoom_change;
		}
	}
	break;

	case GLFW_KEY_UP:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			rotx -= rotch;
		}
	}
	break;

	case GLFW_KEY_DOWN:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			rotx += rotch;
		}
	}
	break;

	case GLFW_KEY_LEFT:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			roty -= rotch;
		}
	}
	break;

	case GLFW_KEY_RIGHT:
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			roty += rotch;
		}
	}
	break;

	case GLFW_KEY_ESCAPE:
	{
		if (action == GLFW_PRESS)
		{
			return_to_console = true;
			zoom = 1.0;
			p = {0, 0};
			rotx = 0;
			roty = 0;
		}
	}
	break;
	}
}

void cursor_position_callback(GLFWwindow *, double, double)
{
}

void cursor_enter_callback(GLFWwindow *, int)
{
}

void mouse_button_callback(GLFWwindow *, int, int, int)
{
}

shadersource parseshader(const std::string &file)
{
	std::ifstream f1(file);
	std::string line;
	std::stringstream ss[2];
	shadertype type = shadertype::NONE;
	while (getline(f1, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = shadertype::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = shadertype::FRAGMENT;
		}
		else
		{
			if ((int)type != -1)
				ss[(int)type] << line << '\n';
		}
	}
	return {ss[0].str(), ss[1].str()};
}

unsigned int compileshader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int res;
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "fail/n" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << std::endl
				  << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int createshader(const std::string &vertexshader, const std::string &fragmentshader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = compileshader(GL_VERTEX_SHADER, vertexshader);
	unsigned int fs = compileshader(GL_FRAGMENT_SHADER, fragmentshader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int create_window(GLFWwindow *&window, int width, int height)
{

	if (!glfwInit())
		return 0;
	window = glfwCreateWindow(width, height, "mesh", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glewInit();
	glfwSetKeyCallback(window, keycallback);
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	//glfwSetWindowRefreshCallback(window, window_refresh_callback);

	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	return 1;
}
