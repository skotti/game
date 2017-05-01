#include <iostream>

#include "debug.h"
#include "engine.h"
#include "model.h"
#include "window.h"
#include "mouse_notifier.h"

const GLuint WIDTH = 800, HEIGHT = 600;

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
