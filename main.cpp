#include <iostream>
#include <sstream>
#include "debug.h"
#include "engine.h"
#include "model.h"
#include "window.h"
#include "mouse_notifier.h"

const GLuint WIDTH = 800, HEIGHT = 600;

GLint nbFrames = 0;
GLfloat lastTime = 0;
void
setWindowFPS (GLFWwindow* win)
{
  // Measure speed
  double currentTime = glfwGetTime();
  nbFrames++;

  if ( currentTime - lastTime >= 1.0 ){ // If last cout was more than 1 sec ago
    char title [256];
    title [255] = '\0';

    snprintf ( title, 255,
                 "%s %s - [FPS: %3.2f]",
                   "Cube", "2.0", 1000.0f / (float)nbFrames );

    glfwSetWindowTitle (win, title);

    nbFrames = 0;
    lastTime += 1.0;
  }
}


int main(int argc, char **argv) {
	
	Window window;
	InputNotifier input_notifier(window);
	MouseNotifier mouse_notifier(window);
	Engine engine(window);
	input_notifier.subscribe(&engine);
	input_notifier.subscribe(&window.getCamera());
	mouse_notifier.subscribe(&window.getCamera());

	engine.initialize();
	
	while(!engine.isExitRequired()) {
		setWindowFPS(window.getGLWindow());
		glfwPollEvents();
		window.clearScreen();
		input_notifier.input();
		mouse_notifier.input();
		engine.update();
		window.swapBuffers();
	}
	
	input_notifier.unsubscribe(&window.getCamera());
	input_notifier.unsubscribe(&engine);
	mouse_notifier.unsubscribe(&window.getCamera());
	
	return 0;
}
