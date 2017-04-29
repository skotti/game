#include "input_notifier.h"

#include "gl_headers.h"
#include "input_event.h"
#include "logger.h"

void InputNotifier::input() {
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