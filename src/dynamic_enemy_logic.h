#ifndef DYNAMIC_ENEMY_OBJECT_H
#define DYNAMIC_ENEMY_OBJECT_H
#include "game_logic.h"
class DynamicEnemyLogic : public GameLogic {
	public:
	DynamicEnemyLogic()
	{
	}
	
	virtual void logic();
	
	virtual ~DynamicEnemyLogic() {}

	static const float S_VISION;
	static const float S_VELOCITY;
	static const float S_ENEMY_WIDTH;
private:

};

#endif