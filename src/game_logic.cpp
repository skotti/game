#include "game_logic.h"
#include "game_object.h"
#include "engine.h"
#include "debug.h"
#include "utils.h"

const float MazeBlockLogic::S_DELTA_H = 0.02;

void MazeBlockLogic::logic()
{
	MazeGenerator& mg = Engine::instance()->getMazeGenerator();
	float desired_height = mg.height(m_i, m_j) * Engine::S_BLOCK_HEIGHT;
	ASSERT(m_game_object != nullptr);
	Vec3f pos = m_game_object->getPos();
	float cur_height = pos[1] + 0.5 * Engine::S_BLOCK_HEIGHT;

	if (cur_height < desired_height) {
		if(cur_height + S_DELTA_H < desired_height) {
			cur_height += S_DELTA_H;
		} else {
			cur_height = desired_height;
		}
	}
	else if (cur_height > desired_height) { 
		if(cur_height - S_DELTA_H > desired_height) {
			cur_height -= S_DELTA_H;
		} else {
			cur_height = desired_height;
		}
	}
	
	pos[1] = cur_height - 0.5 * Engine::S_BLOCK_HEIGHT;
	
	//m_game_object->setSize(Vec3f(Engine::S_BLOCK_WIDTH, Engine::S_BLOCK_HEIGHT, Engine::S_BLOCK_WIDTH));
	m_game_object->setPos(pos);
}
