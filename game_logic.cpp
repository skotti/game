#include "game_logic.h"
#include "game_object.h"
#include "engine.h"
#include "debug.h"
#include "utils.h"

const float MazeBlockLogic::S_DELTA_H = 0.02;

void MazeBlockLogic::logic()
{
	Engine* eng = m_game_object->getEngine();
	MazeGenerator& mg = eng->getMazeGenerator();
	float desired_height = mg.height(m_i, m_j) * Engine::S_BLOCK_HEIGHT;
	ASSERT(m_game_object != nullptr);
	float& cur_height = m_game_object->size()[1];

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
}
