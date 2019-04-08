#ifndef GL_ABASTACTION_HPP
#define GL_ABSTRACTION_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "geometry.h"

struct shadersource
{
	std::string vertex, fragment;
};

enum class shadertype
{
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1
};

inline shadersource parseshader(const std::string &file)
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

inline GLuint compileshader(unsigned int type, const std::string &source)
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
		std::cout << "fail\n"
				  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << std::endl
				  << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

struct color
{
	float r, g, b;
	inline color(float _r = 0, float _g = 0, float _b = 0) : r{_r}, g{_g}, b{_b}
	{
	}
};

inline const color black = {};
inline const color red = {1, 0, 0};
inline const color green = {0, 1, 0};
inline const color blue = {0, 0, 1};
inline const color yellow = {1, 1, 0};
inline const color purple = {1, 0, 1};
inline const color turquoise = {0, 1, 1};
inline const color white = {1, 1, 1};

inline color colors(std::string _color)
{
	static std::unordered_map<std::string, color> m_colors;

	m_colors["black"] = black;
	m_colors["red"] = red;
	m_colors["green"] = green;
	m_colors["blue"] = blue;
	m_colors["yellow"] = yellow;
	m_colors["purple"] = purple;
	m_colors["turquoise"] = turquoise;
	m_colors["white"] = white;

	if (m_colors.find(_color) != m_colors.end())
		return m_colors[_color];
	else
		return m_colors["black"];
}

enum class uniform_types
{
	FLOAT,
	MAT4F
};

struct layout
{
	GLuint id, stride, size;
	GLenum type;
	GLuint offset;

	inline layout(GLuint _id, GLuint _stride, GLuint _size, GLenum _type, GLuint _offset = 0)
		: id{_id}, stride{_stride}, size{_size}, type{_type}, offset{_offset}
	{
	}

	inline std::size_t get_type_size() const
	{
		switch (type)
		{
		case GL_FLOAT:
			return 4;
		case GL_UNSIGNED_INT:
			return 4;
		case GL_UNSIGNED_SHORT:
			return 2;
		default:
			return 0;
		}
	}
};

class data_buffer
{
  private:
	GLuint id;
	GLenum target;
	GLenum usage;
	std::vector<layout> layouts;
	std::size_t size;

  public:
	inline data_buffer(GLenum _target) : target{_target}
	{
		glGenBuffers(1, &id);
	}

	template <typename T>
	inline data_buffer(GLenum _target, const std::vector<T> &data, GLenum _usage = GL_STATIC_DRAW) : target{_target}, usage{_usage}
	{
		glGenBuffers(1, &id);
		glBindBuffer(target, id);
		size = data.size();
		glBufferData(target, sizeof(T) * data.size(), &data[0], _usage);
	}

	inline ~data_buffer()
	{
		glDeleteBuffers(1, &id);
	}

	inline void bind()
	{
		glBindBuffer(target, id);
	}

	inline void unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	inline GLenum get_target()
	{
		return target;
	}

	inline std::size_t get_size()
	{
		return size;
	}

	template <typename T>
	inline void data(const std::vector<T> &data, GLenum _usage = GL_STATIC_DRAW)
	{
		usage = _usage;
		size = data.size();
		glBufferData(target, sizeof(T) * data.size(), &data[0], _usage);
	}

	inline void configure_layout(GLuint m_id, GLuint m_stride, GLuint m_size, GLenum m_type, GLuint m_offset = 0)
	{
		if (target == GL_ARRAY_BUFFER)
			layouts.push_back({m_id, m_stride, m_size, m_type, m_offset});
	}

	inline void add_layouts()
	{
		for (const layout &l : layouts)
			glVertexAttribPointer(l.id, l.stride, l.type, GL_FALSE, l.size * l.get_type_size(), (const void *)(l.offset * l.get_type_size()));
	}

	inline void enable_layouts()
	{
		for (const layout &l : layouts)
			glEnableVertexAttribArray(l.id);
	}
};

class vertex_array
{
  private:
	GLuint id;
	GLenum mode;
	std::vector<data_buffer *> buffers;
	data_buffer *index_buffer = nullptr;

  public:
	inline vertex_array(GLenum _mode) : mode{_mode}
	{
		glGenVertexArrays(1, &id);
	}

	inline ~vertex_array()
	{
		glDeleteVertexArrays(1, &id);
	}

	inline void bind()
	{
		glBindVertexArray(id);
	}

	inline void unbind()
	{
		glBindVertexArray(0);
	}

	inline void bind_buffer(std::vector<data_buffer *> _buffers)
	{
		for (data_buffer *m_b : _buffers)
		{
			buffers.push_back(m_b);
			m_b->bind();
			m_b->enable_layouts();
			m_b->add_layouts();

			if (index_buffer == nullptr && m_b->get_target() == GL_ELEMENT_ARRAY_BUFFER)
				index_buffer = m_b;
		}
	}

	inline void draw()
	{
		bind();
		if (index_buffer != nullptr)
			glDrawElements(mode, index_buffer->get_size(), GL_UNSIGNED_INT, nullptr);
	}
};

class uniform
{
	int location;
	uniform_types type;
	void *data;

  public:
	inline uniform(GLuint shader, const std::string variable, uniform_types _type, void *_data) : type{_type}, data{_data}
	{
		location = glGetUniformLocation(shader, variable.c_str());
		switch (_type)
		{
		case uniform_types::FLOAT:
			glUniform1f(location, *(float *)_data);
			break;

		case uniform_types::MAT4F:
			glUniformMatrix4fv(location, 1, GL_FALSE, (float *)_data);
			break;
		}
	}

	inline void update() const
	{
		switch (type)
		{
		case uniform_types::FLOAT:
			glUniform1f(location, *(float *)data);
			break;

		case uniform_types::MAT4F:
			glUniformMatrix4fv(location, 1, GL_FALSE, (float *)data);
			break;
		}
	}

};

class program
{
  private:
	GLuint id;
	std::vector<uniform> U;

  public:
	inline program(const std::string &shaderpath)
	{
		id = glCreateProgram();
		GLuint vs, fs;
		shadersource src = parseshader(shaderpath);
		vs = compileshader(GL_VERTEX_SHADER, src.vertex);
		fs = compileshader(GL_FRAGMENT_SHADER, src.fragment);
		glAttachShader(id, vs);
		glAttachShader(id, fs);
		glLinkProgram(id);
		glValidateProgram(id);
		glDeleteShader(vs);
		glDeleteShader(fs);
		glUseProgram(id);
	}

	inline void set_uniform(const std::string variable, uniform_types _type, void *_data)
	{
		U.push_back(uniform(id,variable,_type,_data));
	}

	inline void update_uniforms()
	{
		for(const uniform& _u:U)
		{
			_u.update();
		}
	}

	inline GLuint get_id()
	{
		return id;
	}

	inline ~program()
	{
		glDeleteProgram(id);
	}
};

#endif