#include "debug.h"
#include "engine.h"
#include "model.h"
#include "window.h"
#include "mouse_notifier.h"
#include "utils.h"
#include "stl_headers.h"
#include "clock.h"

int main(int argc, char **argv) {
	Engine* engine = Engine::instance();
	
	Clock clock;
	double current = clock.getTime();
	double previous_frame = clock.getTime();
	double previous_second = clock.getTime();
	double lag = 0.0;
	
	Window::TextId fps_id = Window::instance()->registerText("FPS: 0", Vec2f{0.01f, 0.9f}, 0.001f);
	
	while (!engine->isExitRequired())
	{
		current = clock.getTime();
		double elapsed_frame = current - previous_frame;
		double elapsed_second = current - previous_second;
		previous_frame = current;
		lag += elapsed_frame;

		while (lag >= Engine::S_TIME_STEP)
		{
			engine->updateInput();
			engine->updateLogic();
			lag -= Engine::S_TIME_STEP;
		}
		
		if (elapsed_second > 0.2) {
			previous_second = current;
			Window::instance()->setTextString(fps_id, "FPS: " + std::to_string(static_cast<long>(1.0/elapsed_frame)));
		}
		
		engine->updateGraphic();
	}
		
	Window::instance()->destroyText(fps_id);
	
	Engine::destroy();
	
	return 0;
}
