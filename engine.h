#ifndef ENGINE_H
#define ENGINE_H

#include "stl_headers.h"
#include "gl_headers.h"
#include "game_object.h"
#include "input_notifier.h"
#include "yaml-cpp/yaml.h"
#include "logger.h"
#include "model.h"
#include "texture.h"
#include "material.h"
#include "lights.h"
#include "maze_generator.h"

class Engine : public Listener<InputEvent> {

public:
	
	Engine(Window& window) : 
		m_exit_required(false),
		m_window(&window)
	{
		m_logger.setPrefix("Engine:: ");
		m_logger.log("Create engine");
	}
	
	void initialize() {
		m_logger.log("Initialize engine");
		initializeEngine();
		initializeShaders();
		initializeTextures();
		initializeMaterials();
		initializeLightSources();
		initializeModels();
		initializeMaze();
		initializeObjects();
	}
	
	virtual void onEvent(InputEvent event) {
		m_logger.log("Process event");
		if (event == InputEvent::EXIT) {
			m_exit_required = true;
		}
	}
	
	bool isExitRequired() const {
		//m_logger.log("exitcondition checking"); 
		return m_exit_required;
	}
	
	void update() {
		//m_logger.log("Update engine");
		engineLogic();
		for (auto&& obj : m_game_object) {
			obj->update(m_window);
		}
	}
	
	virtual ~Engine() {
		for (auto object : m_game_object) {
			ASSERT(object != nullptr);
			delete object;
		}
		
		for (auto model : m_models) {
			ASSERT(model.second != nullptr);
			delete model.second;
		}
		
		for (auto texture : m_textures) {
			ASSERT(texture.second != nullptr);
			delete texture.second;
		}
		
		for (auto material : m_materials) {
			ASSERT(material.second != nullptr);
			delete material.second;
		}
		
		for (auto light : m_light_sources) {
			ASSERT(light != nullptr);
			delete light;
		}
		
		m_game_object.clear();
		m_models.clear();
		m_textures.clear();
	}
	
	MazeGenerator& getMazeGenerator() { return m_generator; }
	
	static const float S_BLOCK_HEIGHT;
	static const float S_BLOCK_WIDTH;
	static const float S_PLAYER_HEIGHT;
	
	
private:
	
	void registerGameObject(GameObject* game_object) {
		m_logger.log("Register game object");
		game_object->bindEngine(this);
		m_game_object.push_back(game_object);
	}
	
	void initializeEngine();
	void initializeObjects();
	void initializeModels();
	void initializeShaders();
	void initializeTextures();
	void initializeMaterials();
	void initializeLightSources();
	void initializeMaze();
	
	void engineLogic();
	
	bool m_exit_required;
		
	std::list<GameObject*> m_game_object;
	
	std::map<int, Model*> m_models;
	std::map<int, Shader*> m_shaders;
	std::map<int, Texture*> m_textures;
	std::map<int, Material*> m_materials;
	std::vector<Light*> m_light_sources;
	YAML::Node m_config;
	
	MazeGenerator m_generator;
	
	
	Logger m_logger;
	
	Window* m_window;
};

#endif