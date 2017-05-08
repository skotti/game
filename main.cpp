#include <iostream>
#include <sstream>
#include "debug.h"
#include "engine.h"
#include "model.h"
#include "window.h"
#include "mouse_notifier.h"

const GLuint WIDTH = 800, HEIGHT = 600;

int main(int argc, char **argv) {

	Engine engine;

	engine.initialize();
	
	while(!engine.isExitRequired()) {
		engine.update();
	}
	
	return 0;
}
