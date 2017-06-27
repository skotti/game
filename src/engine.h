#ifndef ENGINE_H
#define ENGINE_H

#include "stl_headers.h"
#include "gl_headers.h"
#include "game_object.h"
#include "input_notifier.h"
#include "mouse_notifier.h"
#include "logger.h"
#include "lights.h"
#include "maze_generator.h"
#include "player_logic.h"
#include "window.h"
#include "singleton.h"
#include "menu.h"
#include "platform.h"
#include "random.h"

class Engine : public Listener<InputEvent>, public Singleton<Engine>, public Listener<MenuEvent> {

public:
	
	Engine();
	
	virtual void onEvent(InputEvent event) {
		Logger::instance()->log("Engine process event");
		if (event == InputEvent::MENU) {
			m_menu_mode = !m_menu_mode;
			if (m_menu_mode) {
				glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				m_menu.enable();
			} else {
				glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				m_menu.disable();
			}
		}
	}
	
	virtual void onEvent(MenuEvent event) {
		if (event.m_entry_num == 0) {
			m_new_game = true;
		}
		if (event.m_entry_num == 1) {
			m_exit_required = true;
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
		if (m_new_game) {
			startNewGame(Random::getInt(1, 20), Random::getInt(1, 20));
			m_new_game = false;
		}
		
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
	
	GameObject* getPlayerObject() {
		return m_player_object;
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
	
	void startNewGame(int size_x, int size_y) {
		initializeMaze(size_x, size_y, false);
	}
	
	void registerGameObject(GameObject* game_object) {
		Logger::instance()->log("Register game object");
		m_game_object.push_back(game_object);
	}	
	
	void initializeMaze(int size_x, int size_y, bool simple = true);
	
	void initializeMenu();
	
	void engineLogic();
	void correctPlayerPosition();
	
	bool m_exit_required = false;
		
	std::list<GameObject*> m_game_object;

	struct MazeComponent {
		GameObject* m_maze_block;
		std::array<GameObject*, 4> m_maze_wall = std::array<GameObject*, 4>{0, 0, 0, 0};
	};
	
	std::vector<MazeComponent> m_maze_components;
	
	MazeGenerator m_generator;
	
	InputNotifier m_input_notifier;
	MouseNotifier m_mouse_notifier;
	
	GameObject* m_player_object = nullptr;
	
	Menu m_menu;
	
	bool m_new_game = true;
	bool m_menu_mode = false;
};

#endif
