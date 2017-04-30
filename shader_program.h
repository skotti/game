#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "shader.h"
#include "gl_headers.h"
#include "debug.h"

class ShaderProgram {
	
public:
	ShaderProgram() {
		m_shader_program = GL_CHECK(glCreateProgram());
	}
	
	void compile(){
		GLint success;
		GLchar infoLog[512];
	
		GL_CHECK(glLinkProgram(m_shader_program));
		GL_CHECK(glGetProgramiv(m_shader_program, GL_LINK_STATUS, &success));
		if (!success)
		{
			GL_CHECK(glGetProgramInfoLog(m_shader_program, 512, NULL, infoLog));
			std::cerr << infoLog << std::endl;
			VASSERT(0, "Shader linkage failed");
		}
	}
	
	void attach(Shader* shader) {
		GL_CHECK(glAttachShader(m_shader_program, shader->getGLShader()));
	}
	
	GLuint getShaderProgram() { return m_shader_program; }
	
private:
	GLuint m_shader_program;
};

#endif