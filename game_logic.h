#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "logger.h"

class GameObject;

class GameLogic {
public:
	GameLogic() {
		m_logger.setPrefix("GameObject: ");
		
	}
	
	virtual void logic() = 0;
	virtual ~GameLogic() {}
	
	virtual void setGameObject(GameObject* game_object) {
		m_game_object = game_object;
	}
protected:
	GameObject* m_game_object = nullptr;
	
	Logger m_logger;
};

class EmptyLogic : public GameLogic {
public:
	virtual void logic() {}
	virtual ~EmptyLogic() {}
};

class MazeBlockLogic : public GameLogic {
public:
	MazeBlockLogic(int i, int j) :
		m_i(i),
		m_j(j)
	{

	}
	
	virtual void logic();
	
	virtual ~MazeBlockLogic() {}
	
	static const float S_DELTA_H;
	
private:
	
	int m_i = 0, m_j = 0;
};

#endif