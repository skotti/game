#include "object.h"
#include "logger.h"
#include "gl_headers.h"
#include "debug.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "vector.h"
#include "window.h"

Object::Object(const std::string& file) {
	Logger::instance()->log("Loading model from '%'", file);
	
	Assimp::Importer importer;
	const aiScene* scene;
	if (file == "models/eyeball.obj") {
		
	scene = importer.ReadFile(file, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		
		aiProcess_GenNormals);
	} else {
		scene = importer.ReadFile(file, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		
		aiProcess_GenNormals);
	}
		
	VASSERT(scene, "Can't load model");
	

	int n = scene->mNumMaterials;
	for (unsigned int m = 0;m < scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiString path;  // filename

		aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);

		//Window::instance()->addTexture(path.data);
		if ((Window::S_TEXTURES.find(path.data) == Window::S_TEXTURES.end()) && (texFound != aiReturn_FAILURE))
			Window::S_TEXTURES[path.data] = new Texture(path.data);
		Material* mat = new Material();
		//-----------------------------------------------------------create data for material
		aiColor3D color_diffuse (0.f,0.f,0.f);
		scene->mMaterials[m]->Get(AI_MATKEY_COLOR_DIFFUSE,color_diffuse);
		mat->m_diffuse = Vec3f{color_diffuse.r, color_diffuse.g, color_diffuse.b};
		
		aiColor3D color_specular (0.f,0.f,0.f);
		scene->mMaterials[m]->Get(AI_MATKEY_COLOR_SPECULAR,color_specular);
		mat->m_specular = Vec3f{color_specular.r, color_specular.g, color_specular.b};
		
		aiColor3D color_ambient (0.f,0.f,0.f);
		scene->mMaterials[m]->Get(AI_MATKEY_COLOR_AMBIENT,color_ambient);
		mat->m_ambient = Vec3f{color_ambient.r, color_ambient.g, color_ambient.b};
		
		Vec3f t_ambient = mat->m_ambient;
		Vec3f t_diffuse = mat->m_diffuse;
		Vec3f t_specular = mat->m_specular;
		
		float shininess;
		scene->mMaterials[m]->Get(AI_MATKEY_SHININESS, shininess);
		mat->m_shininess = shininess;
		
		std::string material_name;
		scene->mMaterials[m]->Get(AI_MATKEY_NAME, material_name);
		mat->m_material_name = material_name;
		
		mat->m_texture_name = path.data;
		m_materials.push_back(mat);

	}
	
	for (int i = 0; i < scene->mNumMeshes; i++) {
		Mesh* m= new Mesh();
		m_meshes.push_back(m);
		aiMesh* mesh = scene->mMeshes[i];
		m_meshes[i]->m_vertices.resize(mesh->mNumVertices);
		//-----------------------------------------------------------------set material index
		m_meshes[i]->m_material_index = scene->mMeshes[i]->mMaterialIndex;
		//-----------------------------------------------------------------
		
		for (int j = 0; j < mesh->mNumVertices; ++j) {
			m_meshes[i]->m_vertices.at(j).m_vertex = Vec3<GLfloat>{mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
			m_meshes[i]->m_vertices.at(j).m_norm = Vec3<GLfloat>{mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};
			m_meshes[i]->m_vertices.at(j).m_texture = Vec2<GLfloat>{mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
		//	Logger::instance()->log("Vert %: % % %", i, mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
		}
	
		Logger::instance()->log("numVerices: %", mesh->mNumVertices);
	
		for (int k = 0; k < mesh->mNumFaces; ++k) {
			for (int l = 0; l < mesh->mFaces[i].mNumIndices; ++l) {
				m_meshes[i]->m_indices.push_back(mesh->mFaces[k].mIndices[l]);
			//	Logger::instance()->log("Face %, Ind %: %", k, l, mesh->mFaces[k].mIndices[l]);
			}
		}
	
		GL_CHECK(glGenVertexArrays(1, &m_meshes[i]->m_vao));
		GL_CHECK(glGenBuffers(1, &m_meshes[i]->m_vbo));
		GL_CHECK(glGenBuffers(1, &m_meshes[i]->m_ebo));
		GL_CHECK(glBindVertexArray(m_meshes[i]->m_vao));
		
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_meshes[i]->m_vbo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_meshes[i]->m_vertices.size(), &m_meshes[i]->m_vertices.at(0), GL_STATIC_DRAW));
		
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[i]->m_ebo));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_meshes[i]->m_indices.size(), &m_meshes[i]->m_indices.at(0), GL_STATIC_DRAW));
		
		
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
	for (int i = 0; i < m_materials.size(); i++) {
		delete m_materials[i];
	}
	
	for (int i = 0; i < m_meshes.size(); i++) {
		delete m_meshes[i];
	}
}
