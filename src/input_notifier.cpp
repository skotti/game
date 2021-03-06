#include "input_notifier.h"

#include "gl_headers.h"
#include "input_event.h"
#include "logger.h"
#include "debug.h"

static const int S_BUF_SIZE = 1024;
std::array<bool, S_BUF_SIZE> g_keys = {0};
std::array<bool, S_BUF_SIZE> g_prev_keys = {0};

static bool g_mouse_left = false;
static bool g_prev_mouse_left = false;

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			g_mouse_left = true;
		} else if (action == GLFW_RELEASE) {
			g_mouse_left = false;
		}
	}
}

InputNotifier::InputNotifier() {
	Logger::instance()->log("Create InputNotifier");
	
	g_keys.fill(false);
	glfwSetKeyCallback(Window::instance()->getGLWindow(), key_callback);
	glfwSetMouseButtonCallback(Window::instance()->getGLWindow(), mouse_button_callback);
}

bool InputNotifier::isKeySet(int key) const {
	ASSERT(0 <= key && key < S_BUF_SIZE);
	return g_keys[key];
}

bool InputNotifier::isPressed(int key) const {
	ASSERT(0 <= key && key < S_BUF_SIZE);
	return g_keys[key] && !g_prev_keys[key];
}

bool InputNotifier::isReleased(int key) const {
	ASSERT(0 <= key && key < S_BUF_SIZE);
	return !g_keys[key] && g_prev_keys[key];
}

void InputNotifier::input() {
	if (isKeySet(GLFW_KEY_W)) {
		Logger::instance()->log("Key FORWARD detected");
		notify(InputEvent::FORWARD);
	}
	if (isKeySet(GLFW_KEY_S)) {
		Logger::instance()->log("Key BACKWARD detected");
		notify(InputEvent::BACKWARD);
	} 
	if (isKeySet(GLFW_KEY_A)) {
		Logger::instance()->log("Key SHIFT_LEFT detected");
		notify(InputEvent::SHIFT_LEFT);
	}
	if (isKeySet(GLFW_KEY_D)) {
		Logger::instance()->log("Key SHIFT_RIGHT detected");
		notify(InputEvent::SHIFT_RIGHT);
	}
	if (isKeySet(GLFW_KEY_UP)) {
		Logger::instance()->log("Key UP detected");
		notify(InputEvent::UP);
	}
	if (isKeySet(GLFW_KEY_DOWN)) {
		Logger::instance()->log("Key DOWN detected");
		notify(InputEvent::DOWN);
	} 
	if (isKeySet(GLFW_KEY_LEFT)) {
		Logger::instance()->log("Key ROTATE_LEFT detected");
		notify(InputEvent::ROTATE_LEFT);
	}
	if (isKeySet(GLFW_KEY_RIGHT)) {
		Logger::instance()->log("Key ROTATE_RIGHT detected");
		notify(InputEvent::ROTATE_RIGHT);
	}
	if (isPressed(GLFW_KEY_ESCAPE)) {
		Logger::instance()->log("Key MENU detected");
		notify(InputEvent::MENU);
	}
	if (g_mouse_left && !g_prev_mouse_left) {
		Logger::instance()->log("Key MOUSE_LEFT detected");
		notify(InputEvent::MOUSE_LEFT);
	}
	
	std::copy(g_keys.begin(), g_keys.end(), g_prev_keys.begin());
	g_prev_mouse_left = g_mouse_left;
}