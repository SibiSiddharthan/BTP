#ifndef GL_ABASTACTION_HPP
#define GL_ABSTRACTION_HPP

/**
 * @file gl_abstraction.h
 * @brief An opengl abstraction layer
 * @date 2019-04-17
 * 
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "geometry.h"

/**
 * @struct shadersource 
 * @brief Contains the source code of vertex
 * and fragment shaders
 * 
 * @var shadersource::vertex source of vertex shader
 * @var shadersource::fragment source of fragment shader
 */
struct shadersource
{
	std::string vertex, fragment;
};

/**
 * @brief Enum for shadertpye
 * 
 */
enum class shadertype
{
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1
};

/**
 * @brief From the file parses the shader and splits it into
 * vertex and fragment components
 * 
 * @param file Filepath
 * @return shadersource 
 */
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

/**
 * @brief Compiles the shader and prints error messages if any
 * 
 * @param type Type of shader
 * @param source source of shader
 * @return shader id
 */
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

/**
 * @struct color
 * @brief For simplifying coloring of the components of the display object
 * 
 */
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

/**
 * @brief String based color coding
 * 
 * Allowable values
 * black
 * red
 * green
 * blue
 * yellow
 * purple
 * turquoise
 * white
 * 
 * @param _color the color code
 * @return color 
 */
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

/**
 * @brief Helper for uniforms
 * 
 */
enum class uniform_types
{
	FLOAT,
	MAT4F
};

/**
 * @struct layout
 * @brief A layout of data is distributed in the data buffers
 * Abstraction for glVertexArrtibutes
 * 
 * @var layout::id The id the layout (glVertexAtrribArray)
 * @var layout::stride The stride of data
 * @var layout::size The size of data which is to be processed
 * @var layout::offset The offset from which the data should be read
 * @var layout::type The type of data 
 * 
 */
struct layout
{
	GLuint id, stride, size;
	GLenum type;
	GLuint offset;

	/**
	 * @brief Construct a new layout 
	 * 
	 * @param _id 
	 * @param _stride 
	 * @param _size 
	 * @param _type 
	 * @param _offset 
	 */
	inline layout(GLuint _id, GLuint _stride, GLuint _size, GLenum _type, GLuint _offset = 0)
		: id{_id}, stride{_stride}, size{_size}, type{_type}, offset{_offset}
	{
	}

	/**
	 * @brief Helper for determining size depending upon type
	 * @return (size of data)/stride 
	 */
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

/**
 * @brief Abstraction for GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER
 */
class data_buffer
{
  private:
  	///@brief id glBuffer
	GLuint id;
  	///@brief GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER
	GLenum target;
	///@brief GL_STATIC_DRAW or GL_DYNAMIC_DRAW or GL_STREAM_DRAW
	GLenum usage;
	///@brief Layouts associated with this buffer
	std::vector<layout> layouts;
	///@brief size of data in the buffer
	std::size_t size;

  public:
	/**
	 * @brief Construct a new data buffer 
	 * 
	 * @param _target 
	 */
	inline data_buffer(GLenum _target) : target{_target}
	{
		glGenBuffers(1, &id);
	}

	/**
	 * @brief Construct a data buffer with the data and usage
	 * 
	 * Creates and binds the buffer automatically
	 * 
	 * @tparam T the tpye of data
	 * @param _target target
	 * @param data data to be passed to the buffer
	 * @param _usage 
	 */
	template <typename T>
	inline data_buffer(GLenum _target, const std::vector<T> &data, GLenum _usage = GL_STATIC_DRAW) : target{_target}, usage{_usage}
	{
		glGenBuffers(1, &id);
		glBindBuffer(target, id);
		size = data.size();
		glBufferData(target, sizeof(T) * data.size(), &data[0], _usage);
	}

	/**
	 * @brief Destroy the data buffer 
	 */
	inline ~data_buffer()
	{
		glDeleteBuffers(1, &id);
	}

	/**
	 * @brief Binds the buffer
	 */
	inline void bind()
	{
		glBindBuffer(target, id);
	}

	/**
	 * @brief uninds the buffer
	 */
	inline void unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/**
	 * @brief Helper for vertex_buffers
	 * 
	 * @return GLenum 
	 */
	inline GLenum get_target()
	{
		return target;
	}

	/**
	 * @brief Helper for drawing elements on the screen
	 * 
	 * Drawing elements requires the number of elements to be drawn which is provided by this function
	 * 
	 * @return Number of elements in the buffer 
	 */
	inline std::size_t get_size()
	{
		return size;
	}

	/**
	 * @brief Load the buffer with data
	 * 
	 * @tparam T type
	 * @param data 
	 * @param _usage 
	 */
	template <typename T>
	inline void data(const std::vector<T> &data, GLenum _usage = GL_STATIC_DRAW)
	{
		bind();
		usage = _usage;
		size = data.size();
		glBufferData(target, sizeof(T) * data.size(), &data[0], _usage);
		unbind();
	}

	/**
	 * @brief Configures the layout
	 * 
	 * @param m_id 
	 * @param m_stride 
	 * @param m_size 
	 * @param m_type 
	 * @param m_offset 
	 */
	inline void configure_layout(GLuint m_id, GLuint m_stride, GLuint m_size, GLenum m_type, GLuint m_offset = 0)
	{
		if (target == GL_ARRAY_BUFFER)
			layouts.push_back({m_id, m_stride, m_size, m_type, m_offset});
	}

	/**
	 * @brief adds layouts for the buffer
	 */
	inline void add_layouts()
	{
		for (const layout &l : layouts)
			glVertexAttribPointer(l.id, l.stride, l.type, GL_FALSE, l.size * l.get_type_size(), (const void *)(l.offset * l.get_type_size()));
	}

	/**
	 * @brief Enables the layouts of the buffer
	 */
	inline void enable_layouts()
	{
		for (const layout &l : layouts)
			glEnableVertexAttribArray(l.id);
	}
};

/**
 * @brief Abstraction for glVertexArrays
 */
class vertex_array
{
  private:
	///@brief id glVertexArray
	GLuint id;
	///@brief Drawing mode i.e GL_POINTS, GL_LINES, GL_TRIANGLES
	GLenum mode;
	///@brief vector of pointers to the buffers attached to this vertex array
	std::vector<data_buffer *> buffers;
	///@brief pointer to the index buffer i.e data buffer with target GL_ELEMENT_ARRAY_BUFFER
	data_buffer *index_buffer = nullptr;

  public:
	/**
	 * @brief Construct a new vertex array object
	 * 
	 * @param _mode 
	 */
	inline vertex_array(GLenum _mode) : mode{_mode}
	{
		glGenVertexArrays(1, &id);
	}

	/**
	 * @brief Destroy the vertex array object
	 * 
	 */
	inline ~vertex_array()
	{
		glDeleteVertexArrays(1, &id);
	}

	/**
	 * @brief Binds the vertex array to the framebuffer
	 */
	inline void bind()
	{
		glBindVertexArray(id);
	}

	/**
	 * @brief Unbinds the vertex array from the framebuffer
	 */
	inline void unbind()
	{
		glBindVertexArray(0);
	}

	/**
	 * @brief Adds data buffers to the vertex array
	 * 
	 * NOTE only one of the data buffers must have target GL_ELEMENT_ARRAY_BUFFER
	 * 
	 * @param _buffers vector of data buffers
	 */
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

	/**
	 * @brief Draw command for drawing on the screen
	 */
	inline void draw()
	{
		bind();
		if (index_buffer != nullptr)
			glDrawElements(mode, index_buffer->get_size(), GL_UNSIGNED_INT, nullptr);
	}
};

/**
 * @brief Abstraction for uniforms
 */
class uniform
{
	///@brief Location of the uniform in the shader
	int location;
	///@brief type of uniform MAT4F or FLOAT
	uniform_types type;
	///@brief data of uniform
	void *data;

  public:
	/**
	 * @brief Construct a new uniform object
	 * 
	 * @param shader shader id
	 * @param variable Name of variable
	 * @param _type type of uniform
	 * @param _data data to be passed
	 */
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

	/**
	 * @brief Update the data contents of the uniform
	 */
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

/**
 * @brief Abstraction for glProgram
 */
class program
{
  private:
	///@brief Program id
	GLuint id;
	///@brief vector uniforms associated with the program
	std::vector<uniform> U;

  public:
	/**
	 * @brief Construct a new program with the shader
	 * 
	 * @param shaderpath source of shader
	 */
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

	/**
	 * @brief Set uniforms 
	 * 
	 * @param variable uniform variable name
	 * @param _type 
	 * @param _data 
	 */
	inline void set_uniform(const std::string variable, uniform_types _type, void *_data)
	{
		U.push_back(uniform(id,variable,_type,_data));
	}

	/**
	 * @brief Update the uniforms associated with this object
	 */
	inline void update_uniforms()
	{
		for(const uniform& _u:U)
		{
			_u.update();
		}
	}

	/**
	 * @brief Get the id 
	 * 
	 * @return program id 
	 */
	inline GLuint get_id()
	{
		return id;
	}

	/**
	 * @brief Destroy the program object
	 */
	inline ~program()
	{
		glDeleteProgram(id);
	}
};

#endif