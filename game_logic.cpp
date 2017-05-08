#include "game_logic.h"
#include "game_object.h"
#include "engine.h"
#include "debug.h"
#include "utils.h"

const float MazeBlockLogic::S_DELTA_H = 0.01;

void MazeBlockLogic::logic()
{
	Engine* eng = m_game_object->getEngine();
	MazeGenerator& mg = eng->getMazeGenerator();
	float desired_height = mg.height(m_i, m_j);
	ASSERT(m_game_object != nullptr);
	float& cur_height = m_game_object->size()[1];
	
	if (isEqual(cur_height, desired_height * Engine::S_BLOCK_HEIGHT))
	{}
	else if (cur_height < desired_height * Engine::S_BLOCK_HEIGHT) {
		if(cur_height + S_DELTA_H < desired_height * Engine::S_BLOCK_HEIGHT) {
			cur_height += S_DELTA_H;
		}
	}
	else { 
		if(cur_height - S_DELTA_H > desired_height * Engine::S_BLOCK_HEIGHT) {
			cur_height -= S_DELTA_H;
		}
	}
}
