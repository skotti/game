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
	
	int id = Window::instance()->registerText("OK", Vec2f{0.01f, 0.95f}, 0.001f);
	
	while(!engine->isExitRequired()) {
		
		engine->update();
	}
	
	Window::instance()->destroyText(id);
	
	Engine::destroy();
	
	return 0;
}
