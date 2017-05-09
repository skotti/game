#include <iostream>
#include <sstream>
#include "debug.h"
#include "engine.h"
#include "model.h"
#include "window.h"
#include "mouse_notifier.h"
#include "utils.h"

int main(int argc, char **argv) {
	Engine* engine = Engine::instance();
	
	while(!engine->isExitRequired()) {
		engine->update();
	}
	
	Engine::destroy();
	
	return 0;
}
