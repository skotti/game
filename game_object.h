#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "vector.h"
#include "logger.h"
#include "model.h"
#include "object_view.h"

class Engine;
class Logic;
//class Model;
class Physic;

class GameObject {
	
public:
	GameObject(Vec3f pos, Vec3f angle/*, Logic* logic, Physic* physic*/) :
		m_engine(nullptr),
		m_view(nullptr),
		m_pos(pos),
		m_angle(angle)
	{
		m_logger.setPrefix("GameObject:: ");
		m_logger.log("Create GameObject");
	}
	
	void bindEngine(Engine* engine) {
		m_engine = engine; 
		m_logger.log("Bind engine");
	}
	
	void update(Window* window) {

		m_view->draw(window, m_pos, m_angle);
		//m_logger.log("Update GameObject");
	}
	
	void setView(ObjectView* view) {
		m_view = view;
	}
	virtual ~GameObject() {
		if (m_view != nullptr)
			delete m_view;
	}
private:
	Engine* m_engine;
	ObjectView* m_view;
	
	Vec3f m_pos;
	Vec3f m_angle;
	Logger m_logger;
	/*Logic* m_logic;
	Physic* m_physic;*/
};

#endif