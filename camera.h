#ifndef CAMERA_H
#define CAMERA_H

#include "gl_headers.h"

class Camera {
public:
	
	glm::vec3 getPos() const { return m_camera_pos; }
	glm::vec3 getFront() const { return m_camera_front; }
	glm::vec3 getUp() const { return m_camera_up; }
	
	void setPos(glm::vec3 pos) { m_camera_pos = pos; }
	void setFront(glm::vec3 front) { m_camera_front = front; }
	void setUp(glm::vec3 up) { m_camera_up = up; }
	
private:
	
	glm::vec3 m_camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_camera_up = glm::vec3(0.0f, 1.0f,  0.0f);
};

#endif