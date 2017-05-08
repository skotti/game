#include "game_logic.h"
#include "game_object.h"
#include "engine.h"
#include "debug.h"

void MazeBlockLogic::logic()
{
	Engine* eng = m_game_object->getEngine();
	MazeGenerator& mg = eng->getMazeGenerator();
	float height = mg.height(m_i, m_j);
	ASSERT(m_game_object != nullptr);
	m_game_object->size()[1] = height;
}
