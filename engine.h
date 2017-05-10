#ifndef ENGINE_H
#define ENGINE_H

#include "stl_headers.h"
#include "gl_headers.h"
#include "game_object.h"
#include "input_notifier.h"
#include "mouse_notifier.h"
#include "logger.h"
#include "model.h"
#include "texture.h"
#include "material.h"
#include "lights.h"
#include "maze_generator.h"
#include "player_logic.h"
#include "window.h"
#include "singleton.h"

class Engine : public Listener<InputEvent>, public Singleton<Engine> {

public:
	
	Engine();
	
	virtual void onEvent(InputEvent event) {
		Logger::instance()->log("Engine process event");
		if (event == InputEvent::EXIT) {
			m_exit_required = true;
		}
		if (event == InputEvent::MENU) {
			m_menu_mode = !m_menu_mode;
			if (m_menu_mode) {
				glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			} else {
				glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
	}
	
	bool isExitRequired() const {
		return m_exit_required;
	}
	
	void updateInput() {
		glfwPollEvents();
		InputNotifier::instance()->input();
		MouseNotifier::instance()->input();
	}
	
	void updateLogic() {
		engineLogic();
		for (auto&& obj : m_game_object) { 
			obj->update();
		}
		correctPlayerPosition();
	}
	
	void updateGraphic() {
		Window::instance()->clearScreen();
		Window::instance()->draw();
		Window::instance()->swapBuffers();
	}
	
	virtual ~Engine();
	
	MazeGenerator& getMazeGenerator() { return m_generator; }
	
	static const float S_BLOCK_HEIGHT;
	static const float S_BLOCK_WIDTH;
	static const float S_PLAYER_HEIGHT;
	static const float S_PLAYER_DELTA;
	static const float S_PLAYER_WIDTH;
	static const float S_TIME_STEP;
	
private:
	
	void registerGameObject(GameObject* game_object) {
		Logger::instance()->log("Register game object");
		m_game_object.push_back(game_object);
	}
	
	void initializeMaze();
	
	void engineLogic();
	void correctPlayerPosition();
	
	bool m_exit_required = false;
		
	std::list<GameObject*> m_game_object;
	
	std::vector<GameObject*> m_maze_components;
	
	MazeGenerator m_generator;
	
	InputNotifier m_input_notifier;
	MouseNotifier m_mouse_notifier;
	
	GameObject* m_player_object = nullptr;
	
	bool m_menu_mode = false;
};

#endif