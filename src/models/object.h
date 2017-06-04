#ifndef OBJECT_H
#define OBJECT_H
#include "stl_headers.h"
#include "vector.h"
#include "material.h"
#include "texture.h"

class Object
{
public:
	struct Vertex {
		Vec3<GLfloat> m_vertex;
		Vec3<GLfloat> m_norm;
		Vec2<GLfloat> m_texture;
	};

	
	struct Mesh {
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		
		GLuint m_vbo;
		GLuint m_ebo;
		GLuint m_vao;
		
		GLuint getVBO() { return m_vbo;}
		GLuint getEBO() { return m_ebo;}
		GLuint getVAO() { return m_vao;}
		
		
		int getIndicesSize() {return m_indices.size(); }
		Material* m_material;
		std::string m_texture;
		bool m_has_texture;
	};

	Mesh* getMesh(size_t i) {return m_meshes.at(i);}
	size_t meshesNum() {return m_meshes.size();}
	
	Object(const std::string& file);
	~Object();

private:

	std::vector<Mesh*> m_meshes;
};

#endif // OBJECT_H
