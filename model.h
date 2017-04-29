#ifndef MODEL_H
#define MODEL_H

#include "gl_headers.h"
#include "stl_headers.h"
#include "vector.h"
#include "logger.h"

class Model {

public:
	Model(const std::string& file);
	
	void draw(Vec3f pos, Vec3f angle) {
		m_logger.log("Drawing model at pos = %, %, %, angle = %, %, %", pos[0], pos[1], pos[2], angle[0], angle[1], angle[2]);
	}
	
private:
	GLuint m_vbo;
	GLuint m_vbe;
	GLuint m_vao;
	
	std::vector<GLfloat> m_vertices;
	std::vector<GLfloat> m_norms;
	std::vector<GLfloat> m_textures;
	
	Logger m_logger;
};

#endif