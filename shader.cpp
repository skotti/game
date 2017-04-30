#include "shader.h"
#include "stl_headers.h"

#include "debug.h"
#include "gl_headers.h"

Shader::Shader(const std::string& file_name, ShaderType shader_type)
{
	
	std::ifstream ifs(file_name.c_str());
	std::string content(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);
	GLint success;
	GLchar infoLog[512];
	
	if (shader_type == ShaderType::VERTEX) {
		m_shader = GL_CHECK(glCreateShader(GL_VERTEX_SHADER));
	}
	else {
		m_shader = GL_CHECK(glCreateShader(GL_FRAGMENT_SHADER));
	}
	
	const char *buffer = content.c_str();
	
	GL_CHECK(glShaderSource(m_shader, 1, &buffer, NULL));
	GL_CHECK(glCompileShader(m_shader));

	GL_CHECK(glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success));
	if (!success)
	{
			GL_CHECK(glGetShaderInfoLog(m_shader, 512, NULL, infoLog));
			std::cerr << infoLog << std::endl;
			VASSERT(0, "Shader compilation failed");
	}
}
