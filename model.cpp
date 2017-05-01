#include "model.h"
#include "logger.h"

#include "gl_headers.h"
#include "debug.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

Model::Model(const std::string& file) {
	m_logger.setPrefix("Model:: ");
	m_logger.log("Loading model from '%'", file);
	
	Assimp::Importer importer;
	
	const aiScene* scene = importer.ReadFile(file, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType            |
		aiProcess_GenNormals);
	VASSERT(scene, "Can't load model");
	
	VASSERT(scene->mNumMeshes == 1, "One model required");
	
	aiMesh* mesh = scene->mMeshes[0];
	
	m_vertices.resize(mesh->mNumVertices);
	for (int i = 0; i < mesh->mNumVertices; ++i) {
		m_vertices.at(i).m_vertex = Vec3<GLfloat>{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		m_vertices.at(i).m_norm = Vec3<GLfloat>{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
		m_vertices.at(i).m_texture = Vec2<GLfloat>{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
		m_logger.log("Vert %: % % %", i, mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
	}
	
	m_logger.log("numVerices: %", mesh->mNumVertices);
	
	for (int i = 0; i < mesh->mNumFaces; ++i) {
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
			m_indices.push_back(mesh->mFaces[i].mIndices[j]);
			m_logger.log("Face %, Ind %: %", i, j, mesh->mFaces[i].mIndices[j]);
		}
	}
	
	GL_CHECK(glGenVertexArrays(1, &m_vao));
	GL_CHECK(glGenBuffers(1, &m_vbo));
	GL_CHECK(glGenBuffers(1, &m_ebo));
	GL_CHECK(glBindVertexArray(m_vao));
	
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices.at(0), GL_STATIC_DRAW));
	
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), &m_indices.at(0), GL_STATIC_DRAW));
	
	
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_vertex)));
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_norm)));
	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_texture)));
	GL_CHECK(glEnableVertexAttribArray(2));
	
 
	GL_CHECK(glBindVertexArray(0));
}

Model::~Model()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}
