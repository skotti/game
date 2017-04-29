#include "engine.h"
#include "yaml-cpp/yaml.h"

#include "stl_headers.h"

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
		registerGameObject(new GameObject(pos, angle, m_models.at(object["modelId"].as<int>())));
	}
}



