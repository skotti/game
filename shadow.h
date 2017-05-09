#ifndef SHADOW_H
#define SHADOW_H
#include "shader.h"
#include "shader_program.h"


class Shadow {

public: 
	
	Shadow ();
	
	ShaderProgram* getShaderProgram() {
		return m_shader_program;
	}
	
	GLuint getFBO() {
		return depthMapFBO;
	}
	
	GLuint getMap() {
		return depthMap;
	}
	
	void setShaderProgram(ShaderProgram* shader_program) {
		m_shader_program = shader_program;
	}
	
	~Shadow() {
		if (m_shader_program != nullptr)
			delete m_shader_program;
	}
	
private :
	
	GLuint depthMapFBO;
	GLuint depthMap;
	
	ShaderProgram* m_shader_program;
	
	static const int S_SHADOW_WIDTH;
	static const int S_SHADOW_HEIGHT;
	
};

#endif