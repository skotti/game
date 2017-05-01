#ifndef CAMERA_H
#define CAMERA_H

#include "gl_headers.h"
#include "input_event.h"
#include "listener.h"
class Camera : public Listener<InputEvent> {
public:
	
	virtual void onEvent(InputEvent event);
	
	glm::vec3 getPos() const { return m_camera_pos; }
	glm::vec3 getFront() const { return m_camera_front; }
	glm::vec3 getUp() const { return m_camera_up; }
	
	virtual ~Camera() {}
	
private:
	
	void rotate(GLfloat xoffset, GLfloat yoffset);
	
	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a 
	// direction vector pointing to the right (due to how Eular angles work) 
	// so we initially rotate a bit to the left.
	GLfloat m_yaw = -90.0f;
	GLfloat m_pitch = 0.0f;
	
	glm::vec3 m_camera_pos = glm::vec3(0.0f, 1.0f, 3.0f);
	glm::vec3 m_camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_camera_up = glm::vec3(0.0f, 1.0f,  0.0f);
	
	GLfloat m_shift_speed = 0.2;
	GLfloat m_rotate_speed = 30;
};

#endif