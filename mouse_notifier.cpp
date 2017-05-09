#include "mouse_notifier.h"
#include "mouse_event.h"

float cursor_x;
float cursor_y;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	cursor_x = xpos;
	cursor_y = ypos;
} 

MouseNotifier::MouseNotifier() {
	Logger::instance()->log("Create MouseNotifier");
	glfwSetCursorPosCallback(Window::instance()->getGLWindow(), mouse_callback);
}

void MouseNotifier::input() {
	//m_logger.log("Mouse shift detected");
	MouseEvent mouse_event = {cursor_x, cursor_y};
	notify(mouse_event);
}