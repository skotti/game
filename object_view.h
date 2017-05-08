#ifndef OBJECT_VIEW_H
#define OBJECT_VIEW_H

#include "stl_headers.h"
#include "vector.h"
#include "shader_program.h"
#include "model.h"
#include "window.h"
#include "texture.h"
#include "material.h"
#include "lights.h"

class ObjectView {
	
public:
	ObjectView():
		m_model(nullptr),
		m_shader_program(nullptr),
		m_texture(nullptr),
		m_hasMaterial(false),
		m_hasTexture(false)
	{}
	
	void draw(Window* window, Vec3f pos, Vec3f angle, Vec3f m_size);
	
	void setShaderProgram(ShaderProgram* shader_program) {
		m_shader_program = shader_program;
	}
	
	void setModel(Model* model) {
		m_model = model;
	}
	
	void setTexture(Texture* texture) {
		m_texture = texture;
		m_material = nullptr;
		m_hasTexture = true;
		m_hasMaterial = false;
	}
	
	void setMaterial(Material* material) {
		m_material = material;
		m_texture = nullptr;
		m_hasTexture = false;
		m_hasMaterial = true;
	}
	
	bool hasMaterial() {
		return m_hasMaterial;
	}
	
	bool hasTexture() {
		return m_hasTexture;
	}
	
	void setLightSources(std::vector<Light*> lights) {
		m_lights = lights;
	}
	
	Light* getDirectLight(int i) {
		return m_lights.at(i);
	}
		
	~ObjectView() {
		if (m_shader_program != nullptr)
			delete m_shader_program;
	}
	
private:

	bool m_hasTexture;
	bool m_hasMaterial;
	Model* m_model;
	Texture* m_texture;
	Material* m_material;
	ShaderProgram* m_shader_program;
	std::vector<Light*> m_lights;
};

#endif