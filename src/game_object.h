#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "vector.h"
#include "logger.h"
#include "game_logic.h"
#include "debug.h"
#include "game_logic.h"
#include "game_graphic.h"

class Engine;

class GameObject {
	
public:
	GameObject(GameGraphic* graphic, GameLogic* logic) :
		m_graphic(graphic),
		m_logic(logic)
	{
		Logger::instance()->log("Create GameObject");
		ASSERT(logic != nullptr);
		m_logic->setGameObject(this);
		m_graphic->setGameObject(this);
	}
	
	void update();
	
	Vec3f getPos() const { return m_pos; }
	void setPos(Vec3f val) { m_pos = val; }

	Vec3f getSize() const { return m_size; }
	void setSize(Vec3f val) { m_size = val; }
	
	Vec3f getFront() const { return m_front; }
	void setFront(Vec3f val) { m_front = val; }
	
	Vec3f getUp() const { return m_up; }
	void setUp(Vec3f val) { m_up = val; }
	
	virtual ~GameObject() {
		if (m_graphic != nullptr)
			delete m_graphic;
		if (m_logic != nullptr)
			delete m_logic;
	}
private:

	GameGraphic* m_graphic = nullptr;
	GameLogic* m_logic = nullptr;
	
	Vec3f m_pos = Vec3f{0, 0, 0};
	Vec3f m_size = Vec3f{0, 0, 0};
	Vec3f m_front = Vec3f{0, 0, -1};
	Vec3f m_up = Vec3f{0, 1, 0};
};

#endif