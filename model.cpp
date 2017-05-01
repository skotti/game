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
	
	for (int i = 0; i < mesh->mNumVertices; ++i) {
		m_vertices.push_back(mesh->mVertices[i].x);
		m_vertices.push_back(mesh->mVertices[i].y);
		m_vertices.push_back(mesh->mVertices[i].z);
		m_logger.log("Vert %: % % %", i, mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
	}
	
	m_logger.log("numVerices: %", mesh->mNumVertices);
	
	for (int i = 0; i < mesh->mNumVertices; ++i) {
		m_norms.push_back(mesh->mNormals[i].x);
		m_norms.push_back(mesh->mNormals[i].y);
		m_norms.push_back(mesh->mNormals[i].z);
	}
	
	for (int i = 0; i < mesh->mNumVertices; ++i) {
		m_textures.push_back(mesh->mTextureCoords[0][i].x);
		m_textures.push_back(mesh->mTextureCoords[0][i].y);
		m_textures.push_back(mesh->mTextureCoords[0][i].z);
	}
	
	for (int i = 0; i < mesh->mNumFaces; ++i) {
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
			m_indices.push_back(mesh->mFaces[i].mIndices[j]);
			m_logger.log("Face %, Ind %: %", i, j, mesh->mFaces[i].mIndices[j]);
		}
	}
	
// 	GLfloat vert[] = {
// 		-0.5f, -0.5f, -0.5f,
// 		 0.5f, -0.5f, -0.5f,
// 		 0.5f, -0.5f, 0.5f,
// 		-0.5f, -0.5f, 0.5f,
// 		-0.5f, 0.5f, -0.5f,
// 		 0.5f, 0.5f, -0.5f,
// 		 0.5f, 0.5f, 0.5f,
// 		-0.5f, 0.5f, 0.5f
// 		
// 	};
// 	GLuint ind[] = {
// 		0, 1, 2,
// 		0, 2, 3,
// 		4, 5, 6, 
// 		4, 6, 7,
// 		6, 5, 1,
// 		6, 1, 2,
// 		7, 6, 2, 
// 		7, 2, 3,
// 		7, 6, 0, 
// 		7, 0, 3,
// 		4, 5, 1,
// 		4, 1, 0
// 	};
// 	
// 	m_vertices.resize(8*3);
// 	m_indices.resize(36);
// 	std::copy(vert, &vert[3*8], m_vertices.begin());
// 	std::copy(ind, &ind[36], m_indices.begin());
	
	GL_CHECK(glGenVertexArrays(1, &m_vao));
	GL_CHECK(glGenBuffers(1, &m_vbo));
	GL_CHECK(glGenBuffers(1, &m_ebo));
	GL_CHECK(glBindVertexArray(m_vao));
	
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, /*sizeof(vert)*/ sizeof(GLfloat) * m_vertices.size(), /*vert*/ &m_vertices.at(0), GL_STATIC_DRAW));
	
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER,/* sizeof(ind)*/sizeof(GLuint) * m_indices.size(), /*ind*/ &m_indices.at(0), GL_STATIC_DRAW));
	
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0));
	GL_CHECK(glEnableVertexAttribArray(0));
 
	GL_CHECK(glBindVertexArray(0));
}

Model::~Model()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}
