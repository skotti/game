#include "input_notifier.h"

#include "gl_headers.h"
#include "input_event.h"
#include "logger.h"
#include "debug.h"

static const int S_BUF_SIZE = 1024;
std::array<bool, S_BUF_SIZE> g_keys;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (0 <= key && key < S_BUF_SIZE)
	{
		if (action == GLFW_PRESS) {
			g_keys[key] = true;
		} else if (action == GLFW_RELEASE) {
			g_keys[key] = false;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// TODO
	/*up = false;
	if(firstMouse)
	{
		lastX = xpos;//lastX - last X MOUSE position
		lastY = ypos;//lastY - last Y MOUSE position
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;//compute offset from last MOUSE position
	GLfloat yoffset = lastY - ypos;//compute offset from last MOUSE position
	lastX = xpos;//set new last X position
	lastY = ypos;//set new last Y position

	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;//minor offset
	yoffset *= sensitivity;//minor offset

	yaw   += xoffset;//x offset add an offset to yaw, as we are rotating around y axis and changing our x coord
	pitch += yoffset;//y offset add an offset to yaw, as we are rotating around x axis and changing our y coord

	if(pitch > 89.0f)
		pitch = 89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);*/
}  

InputNotifier::InputNotifier(Window& window) {
	m_logger.setPrefix("InputNotifier:: ");
	m_logger.log("Create InputNotifier");
	
	g_keys.fill(false);
	glfwSetKeyCallback(window.getGLWindow(), key_callback);
	glfwSetCursorPosCallback(window.getGLWindow(), mouse_callback);
}

bool InputNotifier::isKeySet(int key) const {
	ASSERT(0 <= key && key < S_BUF_SIZE);
	return g_keys[key];
}

void InputNotifier::input() {
	glfwPollEvents();
	if (isKeySet(GLFW_KEY_W)) {
		m_logger.log("Key FORWARD detected");
		notify(InputEvent::FORWARD);
	}
	if (isKeySet(GLFW_KEY_S)) {
		m_logger.log("Key BACKWARD detected");
		notify(InputEvent::BACKWARD);
	} 
	if (isKeySet(GLFW_KEY_A)) {
		m_logger.log("Key SHIFT_LEFT detected");
		notify(InputEvent::SHIFT_LEFT);
	}
	if (isKeySet(GLFW_KEY_D)) {
		m_logger.log("Key SHIFT_RIGHT detected");
		notify(InputEvent::SHIFT_RIGHT);
	}
	if (isKeySet(GLFW_KEY_UP)) {
		m_logger.log("Key UP detected");
		notify(InputEvent::UP);
	}
	if (isKeySet(GLFW_KEY_DOWN)) {
		m_logger.log("Key DOWN detected");
		notify(InputEvent::DOWN);
	} 
	if (isKeySet(GLFW_KEY_LEFT)) {
		m_logger.log("Key ROTATE_LEFT detected");
		notify(InputEvent::ROTATE_LEFT);
	}
	if (isKeySet(GLFW_KEY_RIGHT)) {
		m_logger.log("Key ROTATE_RIGHT detected");
		notify(InputEvent::ROTATE_RIGHT);
	}
	if (isKeySet(GLFW_KEY_ESCAPE)) {
		m_logger.log("Key EXIT detected");
		notify(InputEvent::EXIT);
	}
}