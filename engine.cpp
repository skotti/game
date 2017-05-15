#include "engine.h"
#include "yaml-cpp/yaml.h"

#include "stl_headers.h"
#include "shader.h"
#include "math.h"

#include "game_logic.h"
#include "utils.h"
#include "window.h"

#include <random>

const float Engine::S_BLOCK_WIDTH = 1.2;
const float Engine::S_BLOCK_HEIGHT = 0.8;
const float Engine::S_PLAYER_HEIGHT = 2.0 * Window::S_Z_NEAR;
const float Engine::S_PLAYER_DELTA = 0.01;
const float Engine::S_PLAYER_WIDTH = 0.2;

Engine::Engine() : m_exit_required(false)
{
	Logger::instance()->log("Create engine");
	
	InputNotifier::instance()->subscribe(this);
	
	PlayerLogic* player_logic = new PlayerLogic();
	GameGraphic* player_graphic = new EmptyGraphic();
	m_player_object = new GameObject(player_graphic, player_logic);
	
	m_player_object->setSize(Vec3f{S_PLAYER_WIDTH, S_PLAYER_HEIGHT, S_PLAYER_WIDTH});
	registerGameObject(m_player_object);

	initializeMaze();
}

Engine::~Engine()
{
	for (auto object : m_game_object) {
		ASSERT(object != nullptr);
		delete object;
	}
	
	InputNotifier::instance()->unsubscribe(this);
	
	m_game_object.clear();
}
	

/* сделать зазоры*/
void Engine::initializeMaze() {
	Logger::instance()->log("initializeMaze");
	
	int size_x = 5;
	int size_y = 5;
	int start_x = 0;
	int start_y = 0;
	m_generator.generate(size_x, size_y, start_x, start_y);
	m_maze_components.resize(size_x * size_y);
	
	auto start_pos = m_generator.getStart();
	m_player_object->setPos(Vec3f{
		(start_pos.x() + 0.5f)*S_BLOCK_WIDTH,
		m_generator.height(start_pos)*S_BLOCK_HEIGHT/2.0f + S_PLAYER_HEIGHT,
		(start_pos.y() + 0.5f)*S_BLOCK_WIDTH
	});
	
	Vec3f pos{0, 0, 0};
	Vec3f size{0, 0, 0};
	
	for (int j = 0; j < size_y; j++) {
		for (int i = 0; i < size_x; i++) {
			size = Vec3f{
				S_BLOCK_WIDTH, 
				m_generator.height(i, j)*S_BLOCK_HEIGHT,
				S_BLOCK_WIDTH};
				
			pos = Vec3f{
				static_cast<float>(i+0.5)*S_BLOCK_WIDTH, 
				0.0f, 
				static_cast<float>(j+0.5)*S_BLOCK_WIDTH};
			
			GameObject* game_object = new GameObject(new ModelGraphic("cube"), new MazeBlockLogic(i, j));
			game_object->setPos(pos);
			game_object->setSize(size);
			registerGameObject(game_object);
			
			m_maze_components.at(i + j * size_x) = game_object;
		}
	}
}

void Engine::engineLogic() {
	Vec3f pos = m_player_object->getPos();
	
	int cur_cube_i = floor(pos[0] / S_BLOCK_WIDTH);
	int cur_cube_j = floor(pos[2] / S_BLOCK_WIDTH);
	
	Logger::instance()->log("pos = %, %, %, ind = %, %", pos[0], pos[1], pos[2], cur_cube_i, cur_cube_j);
	
	m_generator.setPosition(cur_cube_i, cur_cube_j);
}

void Engine::correctPlayerPosition()
{
	Vec3f cur_player_pos = m_player_object->getPos();
	
	int cur_cube_i = floor(cur_player_pos[0] / S_BLOCK_WIDTH);
	int cur_cube_j = floor(cur_player_pos[2] / S_BLOCK_WIDTH);
	
	int size_x = m_generator.getLen().x();
	Vec3f next_block_height = m_maze_components.at(cur_cube_i + cur_cube_j * size_x)->getSize();
	
	float desired_player_height = next_block_height[1]/2.0f + S_PLAYER_HEIGHT;
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
