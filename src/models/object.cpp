#include "object.h"
#include "logger.h"
#include "gl_headers.h"
#include "debug.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "vector.h"
#include "window.h"

Object::Object(const std::string& file)
{
	Logger::instance()->log("Loading model from '%'", file);
	
	Assimp::Importer importer;
	const aiScene* scene;
	
	scene = importer.ReadFile(file, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
 		//aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals);
		
	VASSERT(scene, "Can't load model");
	
	aiColor3D color_diffuse (0.f,0.f,0.f);
	aiColor3D color_specular (0.f,0.f,0.f);
	aiColor3D color_ambient (0.f,0.f,0.f);
	aiString path;
	Vec3f diffuse, specular, ambient;
	float shininess;
	
	for (int i = 0; i < scene->mNumMeshes; i++) {
		Mesh* m = new Mesh();
		m_meshes.push_back(m);
		aiMesh* mesh = scene->mMeshes[i];
		//------------------------------------------------SET TEXTURE IF OBJ FILE HAS
		aiReturn texFound = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		if (texFound != aiReturn_FAILURE) {
			if (Window::S_TEXTURES.find(path.data) == Window::S_TEXTURES.end()) {
				Window::S_TEXTURES[path.data] = new Texture(path.data);
			}
			m_meshes.at(i)->m_texture = path.data;
			m_meshes.at(i)->m_has_texture = true;
		} else {
			m_meshes.at(i)->m_has_texture = false;
		}
		//------------------------------------------------SET MATERIAL
		
		VASSERT(scene->HasMaterials(), "Can't find material");			
		scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE,color_diffuse);
		scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->Get(AI_MATKEY_COLOR_SPECULAR,color_specular);
		scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->Get(AI_MATKEY_COLOR_AMBIENT,color_ambient);
		diffuse = Vec3f{color_diffuse.r, color_diffuse.g, color_diffuse.b};
		specular = Vec3f{color_specular.r, color_specular.g, color_specular.b};
		ambient = Vec3f{color_ambient.r, color_ambient.g, color_ambient.b};
		scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->Get(AI_MATKEY_SHININESS, shininess);
		
		m_meshes.at(i)->m_material = new Material(ambient, specular, diffuse, shininess);
				
		Logger::instance()->log("numVerices: %", mesh->mNumVertices);
		m_meshes.at(i)->m_vertices.resize(mesh->mNumVertices);
		
		
		for (int j = 0; j < mesh->mNumVertices; ++j) {
			m_meshes.at(i)->m_vertices.at(j).m_vertex = Vec3<GLfloat>{mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
			m_meshes.at(i)->m_vertices.at(j).m_norm = Vec3<GLfloat>{mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};
			if (m_meshes.at(i)->m_has_texture) {
				VASSERT(scene->mMeshes[i]->HasTextureCoords(0), "Can't find texture coords");
				m_meshes.at(i)->m_vertices.at(j).m_texture = Vec2<GLfloat>{mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
			} else { 
				m_meshes.at(i)->m_vertices.at(j).m_texture = Vec2<GLfloat>{0.0, 0.0};
			}
		}
		
		for (int k = 0; k < mesh->mNumFaces; ++k) {
			for (int l = 0; l < mesh->mFaces[i].mNumIndices; ++l) {
				m_meshes.at(i)->m_indices.push_back(mesh->mFaces[k].mIndices[l]);
			}
		}
	
		GL_CHECK(glGenVertexArrays(1, &m_meshes.at(i)->m_vao));
		GL_CHECK(glGenBuffers(1, &m_meshes.at(i)->m_vbo));
		GL_CHECK(glGenBuffers(1, &m_meshes.at(i)->m_ebo));
		GL_CHECK(glBindVertexArray(m_meshes.at(i)->m_vao));
		
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_meshes.at(i)->m_vbo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_meshes.at(i)->m_vertices.size(), &m_meshes.at(i)->m_vertices.at(0), GL_STATIC_DRAW));
		
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes.at(i)->m_ebo));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_meshes.at(i)->m_indices.size(), &m_meshes.at(i)->m_indices.at(0), GL_STATIC_DRAW));
		
		
		GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_vertex)));
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_norm)));
		GL_CHECK(glEnableVertexAttribArray(1));
		GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_texture)));
		GL_CHECK(glEnableVertexAttribArray(2));
	
		GL_CHECK(glBindVertexArray(0));
	
	}
}

Object::~Object()
{
	for (int i = 0; i < m_meshes.size(); i++) {
		delete m_meshes.at(i);
	}
}
