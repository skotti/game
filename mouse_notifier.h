#ifndef MOUSE_NOTIFIER_H
#define MOUSE_NOTIFIER_H


#include "debug.h"
#include "stl_headers.h"
#include "gl_headers.h"
#include "game_object.h"
#include "notifier.h"
#include "input_event.h"
#include "logger.h"
#include "window.h"

class MouseNotifier : public Notifier<MouseEvent> {
public:
	MouseNotifier(Window& window);
	
	void input();
	
private:

	Logger m_logger;
};


#endif