#include <iostream>

#include "debug.h"
#include "engine.h"
#include "model.h"

int main(int argc, char **argv) {

	InputNotifier input_notifier;
	Engine engine;
	input_notifier.subscribe(&engine);

//	Model cube_model(/*"cube.obj"*/);
	
//	GameObject cube_object(cube_model/**/);
	
//	engine.registerGameObject(cube_object);
	engine.initialize();
	
	while(!engine.isExitRequired()) {
		engine.update();
	}
	
	input_notifier.unsubscribe(&engine);
	
	return 0;
}
