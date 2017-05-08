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
//class Model;
class Physic;

class GameObject {
	
public:
	GameObject(Vec3f pos, Vec3f angle, Vec3f size, GameLogic* logic/*, Physic* physic*/) :
		m_engine(nullptr),
		m_view(nullptr),
		m_logic(logic),
		m_pos(pos),
		m_angle(angle),
		m_size(size)
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
	
	void update(Window* window) {
		m_view->draw(window, m_pos, m_angle, m_size);
		m_logic->logic();
		//m_logger.log("Update GameObject");
	}
	
	void setView(ObjectView* view) {
		m_view = view;
		
	}

	Vec3f& size() {
		return m_size;
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
	Engine* m_engine;
	ObjectView* m_view;
	GameLogic* m_logic;
	
	Vec3f m_pos;
	Vec3f m_angle;
	
	Vec3f m_size;
	Logger m_logger;
	
	
	/*Physic* m_physic;*/
};

#endif