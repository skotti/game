#ifndef CAMERA_H
#define CAMERA_H

#include "gl_headers.h"
#include "vector.h"

class Camera {
public:
	
	Vec3f getPos() const { return m_camera_pos; }
	Vec3f getFront() const { return m_camera_front; }
	Vec3f getUp() const { return m_camera_up; }
	
	void setPos(Vec3f pos) { m_camera_pos = pos; }
	void setFront(Vec3f front) { m_camera_front = front; }
	void setUp(Vec3f up) { m_camera_up = up; }
	
private:
	
	Vec3f m_camera_pos = Vec3f{0.0f, 0.0f, 0.0f};
	Vec3f m_camera_front = Vec3f{0.0f, 0.0f, -1.0f};
	Vec3f m_camera_up = Vec3f{0.0f, 1.0f,  0.0f};
};

#endif