#ifndef MODEL_H
#define MODEL_H

#include "gl_headers.h"
#include "stl_headers.h"
#include "vector.h"
#include "logger.h"


class Model {
	struct Vertex {
		Vec3<GLfloat> m_vertex;
		Vec3<GLfloat> m_norm;
		Vec2<GLfloat> m_texture;
	};

public:
	Model(const std::string& file);
	
	GLuint getVBO() { return m_vbo;}
	GLuint getEBO() { return m_ebo;}
	GLuint getVAO() { return m_vao;}
	int getIndicesSize() {return m_indices.size(); }
	~Model();
	
private:
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_vao;
	
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	
	Logger m_logger;
};

#endif