#ifndef MODEL_H
#define MODEL_H

#include "gl_headers.h"
#include "stl_headers.h"
#include "vector.h"
#include "logger.h"

class Model {

public:
	Model(const std::string& file);
	
	GLuint getVBO() { return m_vbo;}
	GLuint getEBO() { return m_ebo;}
	GLuint getVAO() { return m_vao;}
	~Model();
	
private:
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_vao;
	
	std::vector<GLfloat> m_vertices;
	std::vector<GLfloat> m_norms;
	std::vector<GLfloat> m_textures;
	
	Logger m_logger;
};

#endif