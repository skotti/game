#ifndef CAMERA_H
#define CAMERA_H

#include "gl_headers.h"
#include "input_event.h"
#include "mouse_event.h"
#include "listener.h"
#include "vector.h"
class Camera : public Listener<InputEvent>, public Listener<MouseEvent> {
public:
	
	virtual void onEvent(InputEvent event);
	virtual void onEvent(MouseEvent event);
	
	glm::vec3 getPos() const { return m_camera_pos; }
	glm::vec3 getFront() const { return m_camera_front; }
	glm::vec3 getUp() const { return m_camera_up; }
	
	void setPos(glm::vec3 pos) { m_camera_pos = pos; }
	void setFront(glm::vec3 front) { m_camera_front = front; }
	void setUp(glm::vec3 up) { m_camera_up = up; }
	
	virtual ~Camera() {}
	
private:
	
	void rotate(GLfloat xoffset, GLfloat yoffset);
	
	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a 
	// direction vector pointing to the right (due to how Eular angles work) 
	// so we initially rotate a bit to the left.
	GLfloat m_yaw = -90.0f;
	GLfloat m_pitch = 0.0f;
	
	glm::vec3 m_camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_camera_up = glm::vec3(0.0f, 1.0f,  0.0f);
	
	GLfloat m_lastX = m_camera_pos.x;
	GLfloat m_lastY =  m_camera_pos.y;
	bool m_fisrt_mouse = true;
	
	GLfloat m_shift_speed = 0.01;
	GLfloat m_rotate_speed = 30;
};

#endif