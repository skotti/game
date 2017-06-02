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
		int m_material_index;
		
	};

	Mesh* getMesh(size_t i) {return m_meshes.at(i);}
	Material* getMaterial(size_t i) {return m_materials.at(i);}
	size_t meshesNum() {return m_meshes.size();}
	Object(const std::string& file);
	~Object();

private:
	
	int m_num_meshes;
//	std::vector<int> m_num_vertices;
	std::vector<Mesh*> m_meshes;
	std::vector<Material*> m_materials;
};

#endif // OBJECT_H
