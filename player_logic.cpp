#include "player_logic.h"
#include "vector.h"
#include "game_object.h"
#include "engine.h"
#include "window.h"
#include "camera.h"

const GLfloat PlayerLogic::S_SHIFT_SPEED = 0.01f;
const GLfloat PlayerLogic::S_ROTATE_SPEED = 30.0f;

PlayerLogic::PlayerLogic() {
}

void PlayerLogic::setGameObject(GameObject* game_object) {
	GameLogic::setGameObject(game_object);
	assert(m_game_object != nullptr);
	m_last_x = m_game_object->pos()[0];
	m_last_y = m_game_object->pos()[1];
}

void PlayerLogic::logic()
{
	Camera& camera = m_game_object->getEngine()->getWindow().getCamera();
	camera.setPos(toGLMvec3(m_game_object->pos()));
	camera.setFront(toGLMvec3(m_game_object->front()));
	camera.setUp(toGLMvec3(m_game_object->up()));
}

void PlayerLogic::onEvent(InputEvent event)
{
	int size_x = m_game_object->getEngine()->getMazeGenerator().getLen().x();
	int size_y = m_game_object->getEngine()->getMazeGenerator().getLen().y();
	
	glm::vec3 border_pos_plus;
	glm::vec3 border_pos_minus;
	
	glm::vec3 m_player_pos = toGLMvec3(m_game_object->pos());
	glm::vec3 m_player_front = toGLMvec3(m_game_object->front());
	glm::vec3 m_player_up = toGLMvec3(m_game_object->up());
	
	int ind_i = floor(m_player_pos.x / Engine::S_BLOCK_WIDTH);
	int ind_j = floor(m_player_pos.z / Engine::S_BLOCK_WIDTH);
	
	border_pos_plus.x = (ind_i + 1) * Engine::S_BLOCK_WIDTH - Engine::S_PLAYER_WIDTH;
	border_pos_plus.z = (ind_j + 1) * Engine::S_BLOCK_WIDTH - Engine::S_PLAYER_WIDTH;
	border_pos_minus.x = ind_i * Engine::S_BLOCK_WIDTH + Engine::S_PLAYER_WIDTH;
	border_pos_minus.z = ind_j * Engine::S_BLOCK_WIDTH + Engine::S_PLAYER_WIDTH;
	
	MazeGenerator& mg = m_game_object->getEngine()->getMazeGenerator();
	MazeNode& cur_mn = mg.node(ind_i, ind_j);
	
	if (event == InputEvent::FORWARD) {
		m_player_pos = m_player_pos + S_SHIFT_SPEED * glm::normalize(glm::vec3(m_player_front.x, 0, m_player_front.z));
	}
	if (event == InputEvent::BACKWARD) {
		m_player_pos = m_player_pos - S_SHIFT_SPEED * glm::normalize(glm::vec3(m_player_front.x, 0, m_player_front.z));
	}
	if (event == InputEvent::SHIFT_LEFT) {
		m_player_pos = m_player_pos - glm::normalize(glm::cross(m_player_front, m_player_up)) * S_SHIFT_SPEED;
	}
	if (event == InputEvent::SHIFT_RIGHT) {
		m_player_pos = m_player_pos + glm::normalize(glm::cross(m_player_front, m_player_up)) * S_SHIFT_SPEED;
	}
	
	if (m_player_pos.x > border_pos_plus.x && !cur_mn.dir(MazeDir::RIGHT)) {
		m_player_pos.x = border_pos_plus.x;
	}
	if (m_player_pos.x < border_pos_minus.x && !cur_mn.dir(MazeDir::LEFT)) {
		m_player_pos.x = border_pos_minus.x;
	}
	if (m_player_pos.z > border_pos_plus.z && !cur_mn.dir(MazeDir::UP)) {
		m_player_pos.z = border_pos_plus.z;
	}
	if (m_player_pos.z < border_pos_minus.z && !cur_mn.dir(MazeDir::DOWN)) {
		m_player_pos.z = border_pos_minus.z;
	}
	
	if (event == InputEvent::ROTATE_LEFT) {
		rotate(-S_ROTATE_SPEED, 0);
	}
	if (event == InputEvent::ROTATE_RIGHT) {
		rotate(S_ROTATE_SPEED, 0);
	}
	
	m_game_object->pos() = toVec3f(m_player_pos);
	m_game_object->front() = toVec3f(m_player_front);
	m_game_object->up() = toVec3f(m_player_up);
}

void PlayerLogic::onEvent(MouseEvent event)
{
	if(m_first_mouse)
	{
		m_last_x = event.xpos;//lastX - last X MOUSE position
		m_last_y = event.ypos;//lastY - last Y MOUSE position
		m_first_mouse = false;
	}

	GLfloat xoffset = event.xpos - m_last_x;//compute offset from last MOUSE position
	GLfloat yoffset = m_last_y - event.ypos;//compute offset from last MOUSE position
	m_last_x = event.xpos;//set new last X position
	m_last_y = event.ypos;//set new last Y position

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
	
	m_game_object->front() = toVec3f(glm::normalize(front));
}

void PlayerLogic::rotate(GLfloat xoffset, GLfloat yoffset) {
	
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
	
	m_game_object->front() = toVec3f(glm::normalize(front));
}
