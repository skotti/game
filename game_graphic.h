#ifndef MODEL_GRAPHIC_H
#define MODEL_GRAPHIC_H

#include "stl_headers.h"
#include "window.h"
#include "logger.h"
#include "debug.h"

class GameObject;

class GameGraphic {
	
public:
	
	virtual void update() = 0;
	
	virtual void setGameObject(GameObject* game_object) {
		m_game_object = game_object;
	}
	
	virtual ~GameGraphic(){}
protected:
	GameObject* m_game_object = nullptr;
	
	int m_id = -1;
};

class ModelGraphic : public GameGraphic {
		
public:
	
	ModelGraphic(const std::string& window_model_name) {
		m_id = Window::instance()->registerGameObject(window_model_name);
	}
	
	void update();
	
};

class EmptyGraphic : public GameGraphic {

public:
	
	EmptyGraphic() {}
	
	void update(){}
	
};


#endif