#ifndef OBJECT_VIEW_H
#define OBJECT_VIEW_H

#include "stl_headers.h"
#include "vector.h"
#include "shader_program.h"
#include "model.h"
#include "window.h"

class ObjectView {
	
public:
	ObjectView():
		m_model(nullptr),
		m_shader_program(nullptr)
	{}
	
	void draw(Window* window, Vec3f pos, Vec3f angle);
	
	void setShaderProgram(ShaderProgram* shader_program) {
		m_shader_program = shader_program;
	}
	
	void setModel(Model* model) {
		m_model = model;
	}
	
	~ObjectView() {
		if (m_shader_program != nullptr)
			delete m_shader_program;
	}
	
private:

	Model* m_model;
	ShaderProgram* m_shader_program;
};

#endif