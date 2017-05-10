#ifndef PLAYER_LOGIC_H
#define PLAYER_LOGIC_H
#include "listener.h"
#include "input_event.h"
#include "mouse_event.h"
#include "game_logic.h"
#include "gl_headers.h"

class PlayerLogic : public Listener<InputEvent>, public Listener<MouseEvent>, public GameLogic {
public:
	PlayerLogic();
	
	~PlayerLogic();
	
	virtual void onEvent(InputEvent event) override;

	virtual void onEvent(MouseEvent event) override;
	
	virtual void logic() override;
	
	virtual void setGameObject(GameObject* game_object) override;
	
private :
	
	void rotate(GLfloat xoffset, GLfloat yoffset);

	GLfloat m_yaw = -90.0f;
	GLfloat m_pitch = 0.0f;

	GLfloat m_last_x;
	GLfloat m_last_y;
	bool m_first_mouse = true;

	static const GLfloat S_SHIFT_SPEED;
	static const GLfloat S_ROTATE_SPEED;
	
	bool m_disable = false;
};


#endif