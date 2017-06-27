#include "engine.h"
#include "yaml-cpp/yaml.h"

#include "stl_headers.h"
#include "shader.h"
#include "math.h"

#include "game_logic.h"
#include "utils.h"
#include "window.h"
#include "dynamic_enemy_logic.h"

#include <random>

const float Engine::S_BLOCK_WIDTH = 1.2;
const float Engine::S_BLOCK_HEIGHT = 1.5;
const float Engine::S_PLAYER_HEIGHT = 2.0 * Window::S_Z_NEAR;
const float Engine::S_PLAYER_DELTA = 0.01;
const float Engine::S_PLAYER_WIDTH = 0.2;
const float Engine::S_TIME_STEP = 0.01;

Engine::Engine() : m_exit_required(false)
{
	Logger::instance()->log("Create engine");
	
	InputNotifier::instance()->subscribe(this);
	
	PlayerLogic* player_logic = new PlayerLogic();
	GameGraphic* player_graphic = new EmptyGraphic();
	m_player_object = new GameObject(player_graphic, player_logic);
	
	m_player_object->setSize(Vec3f{S_PLAYER_WIDTH, S_PLAYER_HEIGHT, S_PLAYER_WIDTH});
	registerGameObject(m_player_object);
	
// 	DynamicEnemyLogic* enemy_logic = new DynamicEnemyLogic();
// 	ModelGraphic* enemy_graphic = new ModelGraphic("dynamic_enemy");
// 	GameObject* dynamic_enemy = new GameObject(enemy_graphic, enemy_logic);
// 	dynamic_enemy->setSize(Vec3f{0.2, 0.2, 0.2});
// 	dynamic_enemy->setPos(Vec3f{0.0, 20, 0.0});
// 	registerGameObject(dynamic_enemy);
// 	
	
	initializeMenu();
	
	
}

Engine::~Engine()
{
	m_main_menu.unsubscribe(this);
	m_game_menu.unsubscribe(this);
	m_win_menu.unsubscribe(this);
	
	// clear old blocks
	if (m_maze_components.size() != 0) {
		for (auto&& object : m_maze_components) {
			ASSERT(object.m_maze_block != nullptr);
			auto to_delete = std::find(m_game_object.begin(), m_game_object.end(), object.m_maze_block);
			ASSERT(to_delete != m_game_object.end());
			m_game_object.erase(to_delete);
 			delete object.m_maze_block;
			for (int i = 0; i != 4; ++i) {
				if (object.m_maze_wall.at(i) != nullptr) {
					auto to_delete = std::find(m_game_object.begin(), m_game_object.end(), object.m_maze_wall.at(i));
					ASSERT(to_delete != m_game_object.end());
					m_game_object.erase(to_delete);
					delete object.m_maze_wall.at(i);
				}
			}
		}
		m_maze_components.resize(0);
	}
	
	for (auto object : m_game_object) {
		ASSERT(object != nullptr);
		delete object;
	}
	
	InputNotifier::instance()->unsubscribe(this);
	
	m_game_object.clear();
}
	

/* сделать зазоры*/
void Engine::initializeMaze(int size_x, int size_y, bool simple) {
	Logger::instance()->log("initializeMaze");
	
	Logger::instance()->log("Maze size: % %", size_x, size_y);
	
	// clear old blocks
	if (m_maze_components.size() != 0) {
		for (auto&& object : m_maze_components) {
			ASSERT(object.m_maze_block != nullptr);
			auto to_delete = std::find(m_game_object.begin(), m_game_object.end(), object.m_maze_block);
			ASSERT(to_delete != m_game_object.end());
			m_game_object.erase(to_delete);
 			delete object.m_maze_block;
			for (int i = 0; i != 4; ++i) {
				if (object.m_maze_wall.at(i) != nullptr) {
					auto to_delete = std::find(m_game_object.begin(), m_game_object.end(), object.m_maze_wall.at(i));
					ASSERT(to_delete != m_game_object.end());
					m_game_object.erase(to_delete);
					delete object.m_maze_wall.at(i);
				}
			}
		}
		m_maze_components.resize(0);
	}
	
	int start_x = Random::getInt(0, size_x-1);
	int start_y = Random::getInt(0, size_y-1);

	// TODO remove
	//start_x = 0;
	//start_y = 0;
	
	m_generator.generate(size_x, size_y, start_x, start_y, simple);
	m_maze_components.resize(size_x * size_y);
	
	auto start_pos = m_generator.getStart();

	m_player_object->setPos(Vec3f{
		(start_pos.x() + 0.5f)*S_BLOCK_WIDTH,
		m_generator.height(start_pos) + S_PLAYER_HEIGHT,
		(start_pos.y() + 0.5f)*S_BLOCK_WIDTH
	});
	
	Vec3f pos{0, 0, 0};
	Vec3f size{0, 0, 0};
	
	for (int j = 0; j < size_y; j++) {
		for (int i = 0; i < size_x; i++) {
			size = Vec3f{
				S_BLOCK_WIDTH, 
				S_BLOCK_HEIGHT,
				S_BLOCK_WIDTH};
				
			pos = Vec3f{
				static_cast<float>(i+0.5)*S_BLOCK_WIDTH, 
				m_generator.height(MazeIndex(i, j)) - 0.5f * S_BLOCK_HEIGHT, 
				static_cast<float>(j+0.5)*S_BLOCK_WIDTH};
			
			GameObject* game_object = new GameObject(new ModelGraphic("cube"), new MazeBlockLogic(i, j));
			game_object->setPos(pos);
			game_object->setSize(size);
			registerGameObject(game_object);
			
			m_maze_components.at(i + j * size_x).m_maze_block = game_object;
			
			// init walls
			if (simple) {
				for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
					if (!m_generator.node(i, j).dir(static_cast<MazeDir>(dir))) {
						GameObject* wall_game_object = new GameObject(new ModelGraphic("wall"), new EmptyLogic());
						wall_game_object->setSize(size);
						wall_game_object->setUp(Vec3f{0.0, 1.0, 0.0});
						switch (static_cast<MazeDir>(dir)) {
							case MazeDir::LEFT:
								wall_game_object->setPos(Vec3f{
									static_cast<float>(i+0.05)*S_BLOCK_WIDTH,
									0.1f * S_BLOCK_HEIGHT,
									static_cast<float>(j+0.50)*S_BLOCK_WIDTH});
								wall_game_object->setFront(Vec3f{-1.0, 0.0, 0.0});
								break;
							case MazeDir::RIGHT:
								wall_game_object->setPos(Vec3f{
									static_cast<float>(i+0.95)*S_BLOCK_WIDTH,
									0.1f * S_BLOCK_HEIGHT,
									static_cast<float>(j+0.50)*S_BLOCK_WIDTH});
								wall_game_object->setFront(Vec3f{1.0, 0.0, 0.0});
								break;
							case MazeDir::UP:
								wall_game_object->setPos(Vec3f{
									static_cast<float>(i+0.50)*S_BLOCK_WIDTH,
									0.1f * S_BLOCK_HEIGHT,
									static_cast<float>(j+0.95)*S_BLOCK_WIDTH});
								wall_game_object->setFront(Vec3f{0.0, 0.0, 1.0});
								break;
							case MazeDir::DOWN:
								wall_game_object->setPos(Vec3f{
									static_cast<float>(i+0.50)*S_BLOCK_WIDTH,
									0.1f * S_BLOCK_HEIGHT,
									static_cast<float>(j+0.05)*S_BLOCK_WIDTH});
								wall_game_object->setFront(Vec3f{0.0, 0.0, -1.0});
								break;
						}
						m_maze_components.at(i + j * size_x).m_maze_wall.at(dir) = wall_game_object;
						registerGameObject(wall_game_object);
					}
				}
			}
			
			
		}
	}
}

void Engine::initializeMenu()
{
	glfwSetInputMode(Window::instance()->getGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	m_main_menu.subscribe(this);
	m_main_menu.addEntry("New Game");
	m_main_menu.addEntry("Exit");
	m_main_menu.enable();

	m_game_menu.subscribe(this);
	m_game_menu.addEntry("New Game");
	m_game_menu.addEntry("Main Menu");
	m_game_menu.addEntry("Exit");
	m_game_menu.disable();

	m_win_menu.subscribe(this);
	m_win_menu.addEntry("New Game");
	m_win_menu.addEntry("Main Menu");
	m_win_menu.disable();
}

void Engine::engineLogic() {
	
	Vec3f pos = m_player_object->getPos();
	
	int cur_cube_i = floor(pos[0] / S_BLOCK_WIDTH);
	int cur_cube_j = floor(pos[2] / S_BLOCK_WIDTH);
	
	Logger::instance()->log("pos = %, %, %, ind = %, %", pos[0], pos[1], pos[2], cur_cube_i, cur_cube_j);
	
	m_generator.setPosition(cur_cube_i, cur_cube_j, false);
}

void Engine::correctPlayerPosition()
{
	Vec3f cur_player_pos = m_player_object->getPos();
	
	int cur_cube_i = floor(cur_player_pos[0] / S_BLOCK_WIDTH);
	int cur_cube_j = floor(cur_player_pos[2] / S_BLOCK_WIDTH);
	
	int size_x = m_generator.getLen().x();
	Vec3f next_block_height = m_maze_components.at(cur_cube_i + cur_cube_j * size_x).m_maze_block->getPos();
	
	float desired_player_height = next_block_height[1] + 0.5f * Engine::S_BLOCK_HEIGHT + S_PLAYER_HEIGHT;
	float cur_player_height = cur_player_pos[1];
	
	if (cur_player_height < desired_player_height) {
		cur_player_height = desired_player_height;
	}
	else if (cur_player_height > desired_player_height) { 
		if(cur_player_height - S_PLAYER_DELTA > desired_player_height) {
			cur_player_height -= S_PLAYER_DELTA;
		} else {
			cur_player_height = desired_player_height;
		}
	}

	cur_player_pos[1] = cur_player_height;
	
	m_player_object->setPos(cur_player_pos);
}