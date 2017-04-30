#ifndef SHADER_H
#define SHADER_H
#include "stl_headers.h"
#include "gl_headers.h"

enum class ShaderType {
	VERTEX,
	FRAGMENT
};

class Shader {
	
public:
	Shader(const std::string& file_name, ShaderType shader_type);
	
	GLuint getGLShader() { return m_shader;}

private:
	GLuint m_shader;
};



#endif