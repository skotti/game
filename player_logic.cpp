#include "player_logic.h"
#include "vector.h"
#include "game_object.h"
#include "engine.h"
#include "window.h"
#include "camera.h"

const GLfloat PlayerLogic::S_SHIFT_SPEED = 0.01f;
const GLfloat PlayerLogic::S_ROTATE_SPEED = 30.0f;

PlayerLogic::PlayerLogic() {
	
	InputNotifier::instance()->subscribe(this);
	MouseNotifier::instance()->subscribe(this);
}

PlayerLogic::~PlayerLogic() {
	InputNotifier::instance()->unsubscribe(this);
	MouseNotifier::instance()->unsubscribe(this);
}

void PlayerLogic::setGameObject(GameObject* game_object) {
	GameLogic::setGameObject(game_object);
	assert(m_game_object != nullptr);
	m_last_x = m_game_object->getPos()[0];
	m_last_y = m_game_object->getPos()[1];
}

void PlayerLogic::logic()
{
	Camera& camera = Window::instance()->getCamera();
	camera.setPos(m_game_object->getPos());
	camera.setFront(m_game_object->getFront());
	camera.setUp(m_game_object->getUp());
}

void PlayerLogic::onEvent(InputEvent event)
{
	int size_x = Engine::instance()->getMazeGenerator().getLen().x();
	int size_y = Engine::instance()->getMazeGenerator().getLen().y();
	
	Vec3f border_pos_plus{0, 0, 0};
	Vec3f border_pos_minus{0, 0, 0};
	
	glm::vec3 m_player_pos = toGLMvec3(m_game_object->getPos());
	glm::vec3 m_player_front = toGLMvec3(m_game_object->getFront());
	glm::vec3 m_player_up = toGLMvec3(m_game_object->getUp());
	
	int ind_i = floor(m_player_pos[0] / Engine::S_BLOCK_WIDTH);
	int ind_j = floor(m_player_pos[2] / Engine::S_BLOCK_WIDTH);
	
	border_pos_plus[0] = (ind_i + 1) * Engine::S_BLOCK_WIDTH - Engine::S_PLAYER_WIDTH;
	border_pos_plus[2] = (ind_j + 1) * Engine::S_BLOCK_WIDTH - Engine::S_PLAYER_WIDTH;
	border_pos_minus[0] = ind_i * Engine::S_BLOCK_WIDTH + Engine::S_PLAYER_WIDTH;
	border_pos_minus[2] = ind_j * Engine::S_BLOCK_WIDTH + Engine::S_PLAYER_WIDTH;
	
	MazeGenerator& mg = Engine::instance()->getMazeGenerator();
	MazeNode& cur_mn = mg.node(ind_i, ind_j);
	
	if (event == InputEvent::MENU) {
		m_disable = !m_disable;
	}
	if (!m_disable) {
		if (event == InputEvent::FORWARD) {
			m_player_pos = m_player_pos + S_SHIFT_SPEED * glm::normalize(glm::vec3(m_player_front[0], 0, m_player_front[2]));
		}
		if (event == InputEvent::BACKWARD) {
			m_player_pos = m_player_pos - S_SHIFT_SPEED * glm::normalize(glm::vec3(m_player_front[0], 0, m_player_front[2]));
		}
		if (event == InputEvent::SHIFT_LEFT) {
			m_player_pos = m_player_pos - glm::normalize(glm::cross(m_player_front, m_player_up)) * S_SHIFT_SPEED;
		}
		if (event == InputEvent::SHIFT_RIGHT) {
			m_player_pos = m_player_pos + glm::normalize(glm::cross(m_player_front, m_player_up)) * S_SHIFT_SPEED;
		}
		
		if (m_player_pos[0] > border_pos_plus[0] && !cur_mn.dir(MazeDir::RIGHT)) {
			m_player_pos[0] = border_pos_plus[0];
		}
		if (m_player_pos[0] < border_pos_minus[0] && !cur_mn.dir(MazeDir::LEFT)) {
			m_player_pos[0] = border_pos_minus[0];
		}
		if (m_player_pos[2] > border_pos_plus[2] && !cur_mn.dir(MazeDir::UP)) {
			m_player_pos[2] = border_pos_plus[2];
		}
		if (m_player_pos[2] < border_pos_minus[2] && !cur_mn.dir(MazeDir::DOWN)) {
			m_player_pos[2] = border_pos_minus[2];
		}
		
		if (event == InputEvent::ROTATE_LEFT) {
			rotate(-S_ROTATE_SPEED, 0);
		}
		if (event == InputEvent::ROTATE_RIGHT) {
			rotate(S_ROTATE_SPEED, 0);
		}
		if (event == InputEvent::UP) {
			rotate(0, S_ROTATE_SPEED);
		}
		if (event == InputEvent::DOWN) {
			rotate(0, -S_ROTATE_SPEED);
		}
	}
	
	m_game_object->setPos(toVec3f(m_player_pos));
	m_game_object->setFront(toVec3f(m_player_front));
	m_game_object->setUp(toVec3f(m_player_up));
}

void PlayerLogic::onEvent(MouseEvent event)
{
	
	if(m_first_mouse)
	{
		m_last_x = event.xpos;//lastX - last X MOUSE position
		m_last_y = event.ypos;//lastY - last Y MOUSE position
		m_first_mouse = false;
	}

	float xoffset = event.xpos - m_last_x;//compute offset from last MOUSE position
	float yoffset = m_last_y - event.ypos;//compute offset from last MOUSE position
	m_last_x = event.xpos;//set new last X position
	m_last_y = event.ypos;//set new last Y position

	if (m_disable) return;
	
	float sensitivity = 0.05;
	xoffset *= sensitivity;//minor offset
	yoffset *= sensitivity;//minor offset

	m_yaw   += xoffset;//x offset add an offset to yaw, as we are rotating around y axis and changing our x coord
	m_pitch += yoffset;//y offset add an offset to yaw, as we are rotating around x axis and changing our y coord

	if(m_pitch > 89.0f)
		m_pitch = 89.0f;
	if(m_pitch < -89.0f)
		m_pitch = -89.0f;
	
	glm::vec3 front(
		cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
		sin(glm::radians(m_pitch)),
		sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
	
	m_game_object->setFront(toVec3f(glm::normalize(front)));
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

	glm::vec3 front(
		cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
		sin(glm::radians(m_pitch)),
		sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
	
	m_game_object->setFront(toVec3f(glm::normalize(front)));
}
