#ifndef WINDOW_H
#define WINDOW_H
#include "gl_headers.h"
#include "logger.h"
#include "debug.h"
#include "camera.h"

class Window {
	
public:
	Window();
	~Window();
	GLFWwindow* getGLWindow() {
		return m_window;
	}
	void clearScreen() {
		GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	
	void swapBuffers() {
		glfwSwapBuffers(m_window);
	}
	
	Camera& getCamera() { return m_camera; }

private:
	static GLuint S_WIDTH, S_HEIGHT; 
	GLFWwindow* m_window;
	Logger m_logger;
	
	Camera m_camera;
};

#endif