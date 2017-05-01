#include "engine.h"
#include "yaml-cpp/yaml.h"

#include "stl_headers.h"
#include "shader.h"

void Engine::initializeEngine()
{
	m_config = YAML::LoadFile("config.yaml");
	m_logger.log("Initialize engine");
	
}

void Engine::initializeModels()
{
	m_logger.log("Initialize models");
	for (const auto& model : m_config["models"]) {
		m_logger.log("Found model : file = %, id = %", model["file"].as<std::string>(), model["id"].as<int>());
		auto pair = m_models.insert(std::pair<int, Model*>(model["id"].as<int>(), new Model(model["file"].as<std::string>())));
		DASSERT(pair.second, "Cannot insert model twice");
	}
	
}

void Engine::initializeObjects()
{
	for (const auto& object : m_config["objects"]) {
		Vec3f pos = {
			object["pos"][0].as<float>(), 
			object["pos"][1].as<float>(), 
			object["pos"][2].as<float>()};
		Vec3f angle = {
			object["angle"][0].as<float>(), 
			object["angle"][1].as<float>(), 
			object["angle"][2].as<float>()};	
		m_logger.log("Found model : pos = %, %, %, id = %, %, %", pos[0], pos[1], pos[2], angle[0], angle[1], angle[2]);
		GameObject* game_object = new GameObject(pos, angle);
		
		ShaderProgram* shader_program = new ShaderProgram();
		for (const auto& shader : object["shadersId"])
			shader_program->attach(m_shaders.at(shader.as<int>()));
		shader_program->compile();
		
		ObjectView* object_view = new ObjectView();
		object_view->setModel(m_models.at(object["modelId"].as<int>()));
		object_view->setShaderProgram(shader_program);
		object_view->setTexture(m_textures.at(object["textureId"].as<int>()));
		game_object->setView(object_view);
		
		registerGameObject(game_object);
	}
}

void Engine::initializeShaders()
{
	m_logger.log("Initialize shaders");
	for (const auto& shader : m_config["shaders"]["vertex"]) {
		m_logger.log("Found vertex shader : file = %, id = %", shader["file"].as<std::string>(), shader["id"].as<int>());
		auto pair = m_shaders.insert(std::pair<int, Shader*>(shader["id"].as<int>(), new Shader(shader["file"].as<std::string>(), ShaderType::VERTEX)));
		DASSERT(pair.second, "Cannot insert vertex shader twice");
	}
	
	for (const auto& shader : m_config["shaders"]["fragment"]) {
		m_logger.log("Found vertex shader : file = %, id = %", shader["file"].as<std::string>(), shader["id"].as<int>());
		auto pair = m_shaders.insert(std::pair<int, Shader*>(shader["id"].as<int>(), new Shader(shader["file"].as<std::string>(), ShaderType::FRAGMENT)));
		DASSERT(pair.second, "Cannot insert fragment shader twice");
	}
}


void Engine::initializeTextures()
{
	m_logger.log("Initialize shaders");
	for (const auto& texture : m_config["textures"]) {
		m_logger.log("Found texture : file = %, id = %", texture["file"].as<std::string>(), texture["id"].as<int>());
		auto pair = m_textures.insert(std::pair<int, Texture*>(texture["id"].as<int>(), new Texture(texture["file"].as<std::string>())));
		DASSERT(pair.second, "Cannot insert texture twice");
	}
}




