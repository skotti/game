#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "vector.h"
#include "logger.h"
#include "model.h"
#include "object_view.h"
#include "game_logic.h"
#include "debug.h"
#include "game_logic.h"

class Engine;

class GameObject {
	
public:
	GameObject(GameLogic* logic) :
		m_logic(logic)
	{
		m_logger.setPrefix("GameObject:: ");
		m_logger.log("Create GameObject");
		ASSERT(logic != nullptr);
		m_logic->setGameObject(this);
	}
	
	GameObject(Vec3f pos, Vec3f size, Vec3f front, Vec3f up, GameLogic* logic/*, Physic* physic*/) :
		m_pos(pos),
		m_size(size),
		m_front(front),
		m_up(up),
		m_logic(logic)
	{
		m_logger.setPrefix("GameObject:: ");
		m_logger.log("Create GameObject");
		ASSERT(logic != nullptr);
		m_logic->setGameObject(this);
	}
	
	void bindEngine(Engine* engine) {
		m_engine = engine; 
		m_logger.log("Bind engine");
	}
	
	void update();
	
	void setView(ObjectView* view) {
		m_view = view;
		
	}
	
	Vec3f& pos() {
		return m_pos;
	}

	Vec3f& size() {
		return m_size;
	}
	
	Vec3f& front() {
		return m_front;
	}
	
	Vec3f& up() {
		return m_up;
	}
	
	Engine* getEngine() {
		return m_engine;
	}
	
	virtual ~GameObject() {
		if (m_view != nullptr)
			delete m_view;
		if (m_logic != nullptr)
			delete m_logic;
	}
private:
	Engine* m_engine = nullptr;
	ObjectView* m_view = nullptr;
	GameLogic* m_logic = nullptr;
	
	Vec3f m_pos = Vec3f{0, 0, 0};
	
	Vec3f m_size = Vec3f{0, 0, 0};
	
	Vec3f m_front = Vec3f{0, 0, -1};
	Vec3f m_up = Vec3f{0, 1, 0};

	Logger m_logger;
	
	
	/*Physic* m_physic;*/
};

#endif