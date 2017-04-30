#include "window.h"
#include "debug.h"

#include "logger.h"

GLuint Window::S_WIDTH = 800;
GLuint Window::S_HEIGHT = 600; 

Window::Window() {
	m_logger.setPrefix("Window:: ");
	m_logger.log("Create window");
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(S_WIDTH, S_HEIGHT, "Cube Game", nullptr, nullptr);
	ASSERT(m_window != nullptr);
	glfwMakeContextCurrent(m_window);
	
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	while(glGetError()) {} // eat error produced by glew

	GL_CHECK(glViewport(0, 0, S_WIDTH, S_HEIGHT));
	
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthFunc(GL_LESS));
}
	
Window::~Window() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}