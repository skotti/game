#ifndef ENGINE_H
#define ENGINE_H

#include "stl_headers.h"
#include "game_object.h"
#include "input_notifier.h"
#include "yaml-cpp/yaml.h"
#include "logger.h"
#include "model.h"

class Engine : public Listener<InputEvent> {

public:
	
	Engine() : 
		m_exit_required(false)
	{
		m_logger.setPrefix("Engine:: ");
		m_logger.log("Create engine");
	}
	
	void initialize() {
		m_logger.log("Initialize engine");
		initializeEngine();
		initializeModels();
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
		for (auto&& obj : m_game_object) {
			obj->update();
		}
		engineLogic();
	}
	
	
private:
	
	void registerGameObject(GameObject* game_object) {
 		m_logger.log("Register game object");
		m_game_object.push_back(game_object);
 	}
	
	
	void initializeEngine();
	void initializeObjects();
	void initializeModels();
	
	void engineLogic(){/*TODO*/}
	
	bool m_exit_required;
		
	std::list<GameObject*> m_game_object;
	std::map<int, Model*> m_models;
	
	YAML::Node m_config;
	
	Logger m_logger;
};

#endif