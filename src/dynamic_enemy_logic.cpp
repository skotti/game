#include "dynamic_enemy_logic.h"
#include "engine.h"
#include "gl_headers.h"
#include "vector.h"
const float DynamicEnemyLogic::S_VISION = 0.1;
const float DynamicEnemyLogic::S_VELOCITY = 0.02;
const float DynamicEnemyLogic::S_ENEMY_WIDTH = 0.2;

void DynamicEnemyLogic::logic()
{
	GameObject* player = Engine::instance()->getPlayerObject();
	
	glm::vec3 scale = toGLMvec3(player->getSize());
	scale.y = 0;
	float half_width = glm::length(scale);
	glm::vec3 player_pos = toGLMvec3(player->getPos());
	player_pos.z += 1.0;
	
	glm::vec3 pos = toGLMvec3(m_game_object->getPos());
	
// 	int ind_i = floor(pos[0] / Engine::S_BLOCK_WIDTH);
// 	int ind_j = floor(pos[2] / Engine::S_BLOCK_WIDTH);
	
// 	MazeGenerator& mg = Engine::instance()->getMazeGenerator();
// 	MazeNode& cur_mn = mg.node(ind_i, ind_j);

	glm::vec3 border_pos_plus = glm::vec3(0, 0, 0);
	glm::vec3 border_pos_minus = glm::vec3(0, 0, 0);
// 	border_pos_plus.x = (ind_i + 1) * Engine::S_BLOCK_WIDTH - half_width;
// 	border_pos_plus.z = (ind_j + 1) * Engine::S_BLOCK_WIDTH - half_width;
// 	border_pos_minus.x = ind_i * Engine::S_BLOCK_WIDTH + half_width;
// 	border_pos_minus.z = ind_j * Engine::S_BLOCK_WIDTH + half_width;
	
	float distance = glm::distance(pos, player_pos);

	glm::vec3 direction = glm::normalize(player_pos - pos);
	
	Logger::instance()->log("enemy pos: % % %", pos.x, pos.y, pos.z);
	Logger::instance()->log("enemy player: % % %", player_pos.x, player_pos.y, player_pos.z);
	
// 	if (distance < S_VISION) {
	
	if (distance < S_VELOCITY) {
		pos = player_pos;
	} else {
		pos += direction * S_VELOCITY;
	}
// 		
// 	}
	
// 	if (pos[0] > border_pos_plus[0] && !cur_mn.dir(MazeDir::RIGHT)) {
// 		pos[0] = border_pos_plus[0];
// 	}
// 	if (pos[0] < border_pos_minus[0] && !cur_mn.dir(MazeDir::LEFT)) {
// 		pos[0] = border_pos_minus[0];
// 	}
// 	if (pos[2] > border_pos_plus[2] && !cur_mn.dir(MazeDir::UP)) {
// 		pos[2] = border_pos_plus[2];
// 	}
// 	if (pos[2] < border_pos_minus[2] && !cur_mn.dir(MazeDir::DOWN)) {
// 		pos[2] = border_pos_minus[2];
// 	}

	glm::vec3 dir2 = glm::normalize(toGLMvec3(player->getPos()) - pos);

	m_game_object->setFront(toVec3f(dir2));
	m_game_object->setPos(toVec3f(pos));
}
