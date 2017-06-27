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
#include "game_state.h"
#include "audio/player.h"

class Engine : public Listener<InputEvent>, public Singleton<Engine>, public Listener<MenuEvent> {

public:
	
	Engine();
	
	virtual void onEvent(InputEvent event) {
		Logger::instance()->log("Engine process event");
		switch (m_game_state) 
		{
		case GameState::MainMenu:
			break;
		case GameState::Game:
			if (event == InputEvent::MENU)
			{
				setGameState(GameState::GameMenu);
			}
			break;
		case GameState::GameMenu:
			if (event == InputEvent::MENU)
			{
				setGameState(GameState::Game);
			}
			break;
		case GameState::WinScreen:
			if (event == InputEvent::MENU)
			{
				setGameState(GameState::MainMenu);
			}
			break;
		}
	}
	
	virtual void onEvent(MenuEvent event) {
		switch (m_game_state)
		{
			case GameState::Game:
				break;
			case GameState::MainMenu:
				if (event.m_entry_num == 0) {
					m_new_game = true;
					setGameState(GameState::Game);
				}
				else if (event.m_entry_num == 1) {
					m_exit_required = true;
				}
				break;
			case GameState::GameMenu:
				if (event.m_entry_num == 0) {
					m_new_game = true;
					setGameState(GameState::Game);
				}
				else if (event.m_entry_num == 1) {
					setGameState(GameState::MainMenu);
				}
				else if (event.m_entry_num == 2) {
					m_exit_required = true;
				}
				break;
			case GameState::WinScreen:
				if (event.m_entry_num == 0) {
					m_new_game = true;
					setGameState(GameState::Game);
				}
				else if (event.m_entry_num == 1) {
					setGameState(GameState::MainMenu);
				}
				break;
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
			startNewGame(Random::getInt(2, 10), Random::getInt(2, 10));
			m_new_game = false;
		}

		// update game state 
		
		if (m_game_state != m_next_game_state)
		switch (m_next_game_state) 
		{
		case GameState::MainMenu:
			glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_game_menu->disable();
			m_main_menu->enable();
			m_win_menu->disable();
			Window::instance()->showText(m_game_title);
			Window::instance()->hideText(m_you_win);
			m_audio_player.stop(m_ambient_background);
			m_audio_player.stop(m_audio_win);
			break;
		case GameState::Game:
			glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_game_menu->disable();
			m_main_menu->disable();
			m_win_menu->disable();
			Window::instance()->hideText(m_game_title);
			Window::instance()->hideText(m_you_win);
			m_audio_player.play(m_ambient_background);
			m_audio_player.stop(m_audio_win);
			break;
		case GameState::GameMenu:
			glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_game_menu->enable();
			m_main_menu->disable();
			m_win_menu->disable();
			Window::instance()->hideText(m_game_title);
			Window::instance()->hideText(m_you_win);
			m_audio_player.pause(m_ambient_background);
			m_audio_player.stop(m_audio_win);
			break;
		case GameState::WinScreen:
			glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_game_menu->disable();
			m_main_menu->disable();
			m_win_menu->enable();
			Window::instance()->hideText(m_game_title);
			Window::instance()->showText(m_you_win);
			m_audio_player.stop(m_ambient_background);
			m_audio_player.play(m_audio_win);
			break;
		}
		m_game_state = m_next_game_state;
		
		// end of updating game state

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
	
	GameState getGameState() const { return m_game_state; }
	void setGameState(GameState state) { 
		m_next_game_state = state;
	}

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
	
	Menu* m_game_menu;
	Menu* m_main_menu;
	Menu* m_win_menu;
	
	bool m_new_game = true;

	GameState m_game_state = GameState::MainMenu;
	GameState m_next_game_state = GameState::MainMenu;

	Window::TextId m_game_title;
	Window::TextId m_you_win;

	Player m_audio_player;
	Player::SourceId m_ambient_background;
	Player::SourceId m_audio_win;
};

#endif
