#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "vector.h"
#include "logger.h"

class Engine;
class Logic;
class Model;
class Physic;

class GameObject {
	
public:
	GameObject(Vec3f pos, Vec3f angle, Model* model/*, Logic* logic, Physic* physic*/) :
		m_engine(nullptr),
		m_model(model),
		m_pos(pos),
		m_angle(angle)/*,
		m_logic(logic),
		m_physic(physic)*/
	{
		m_logger.setPrefix("GameObject:: ");
		m_logger.log("Create GameObject");
	}
	
	void bindEngine(Engine* engine) {
		m_engine = engine; 
		m_logger.log("Bind engine");
	}
	
	void update() {
// 		m_logic->logic(); 
// 		m_physic->physics();
//		drawModel(m_model);
		//m_logger.log("Update GameObject");
	}
	
	Model* getModel() const {
		return m_model;
	}
	
	virtual ~GameObject() {}
private:
	Engine* m_engine;
	Model* m_model;
	Vec3f m_pos;
	Vec3f m_angle;
	Logger m_logger;
	/*Logic* m_logic;
	Physic* m_physic;*/
};

#endif