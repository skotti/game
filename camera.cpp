#include "camera.h"

void Camera::onEvent(InputEvent event)
{
	if (event == InputEvent::FORWARD) {
		 m_camera_pos = m_camera_pos + m_shift_speed * m_camera_front;
  }
  if (event == InputEvent::BACKWARD) {
		 m_camera_pos = m_camera_pos - m_shift_speed * m_camera_front;;
  }
  if (event == InputEvent::SHIFT_LEFT) {
		 m_camera_pos = m_camera_pos - glm::normalize(glm::cross(m_camera_front, m_camera_up)) * m_shift_speed;
  }
  if (event == InputEvent::SHIFT_RIGHT) {
		 m_camera_pos = m_camera_pos + glm::normalize(glm::cross(m_camera_front, m_camera_up)) * m_shift_speed;
  }
  if (event == InputEvent::UP) {
		rotate(0, m_rotate_speed);
  }
  if (event == InputEvent::DOWN) {
		rotate(0, -m_rotate_speed);
  }
  if (event == InputEvent::ROTATE_LEFT) {
		rotate(-m_rotate_speed, 0);
  }
  if (event == InputEvent::ROTATE_RIGHT) {
		rotate(m_rotate_speed, 0);
  }
}

void Camera::onEvent(MouseEvent event)
{
	if(m_fisrt_mouse)
	{
		m_lastX = event.xpos;//lastX - last X MOUSE position
		m_lastY = event.ypos;//lastY - last Y MOUSE position
		m_fisrt_mouse = false;
	}

	GLfloat xoffset = event.xpos - m_lastX;//compute offset from last MOUSE position
	GLfloat yoffset = m_lastY - event.ypos;//compute offset from last MOUSE position
	m_lastX = event.xpos;//set new last X position
	m_lastY = event.ypos;//set new last Y position

	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;//minor offset
	yoffset *= sensitivity;//minor offset

	m_yaw   += xoffset;//x offset add an offset to yaw, as we are rotating around y axis and changing our x coord
	m_pitch += yoffset;//y offset add an offset to yaw, as we are rotating around x axis and changing our y coord

	if(m_pitch > 89.0f)
		m_pitch = 89.0f;
	if(m_pitch < -89.0f)
		m_pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_camera_front = glm::normalize(front);
}

void Camera::rotate(GLfloat xoffset, GLfloat yoffset) {
	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;//minor offset
	yoffset *= sensitivity;//minor offset

	m_yaw += xoffset;//x offset add an offset to yaw, as we are rotating around y axis and changing our x coord
	m_pitch += yoffset;//y offset add an offset to yaw, as we are rotating around x axis and changing our y coord

	if(m_pitch > 89.0f)
		m_pitch = 89.0f;
	if(m_pitch < -89.0f)
		m_pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_camera_front = glm::normalize(front);
}