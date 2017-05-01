#include <iostream>

#include "debug.h"
#include "engine.h"
#include "model.h"
#include "window.h"

const GLuint WIDTH = 800, HEIGHT = 600;

int main(int argc, char **argv) {
	
	Window window;
	InputNotifier input_notifier(window);
	Engine engine(window);
	input_notifier.subscribe(&engine);
	input_notifier.subscribe(&window.getCamera());

	engine.initialize();
	
	while(!engine.isExitRequired()) {
		window.clearScreen();
		input_notifier.input();
		engine.update();
		window.swapBuffers();
	}
	
	input_notifier.unsubscribe(&window.getCamera());
	input_notifier.unsubscribe(&engine);
	
	return 0;
}
