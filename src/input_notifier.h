#ifndef INPUT_NOTIFIER
#define INPUT_NOTIFIER

#include "debug.h"
#include "stl_headers.h"
#include "gl_headers.h"
#include "game_object.h"
#include "notifier.h"
#include "input_event.h"
#include "logger.h"
#include "window.h"
#include "singleton.h"

class InputNotifier : public Notifier<InputEvent>, public Singleton<InputNotifier> {
public:
	InputNotifier();
	
	void input();
	
private:
	bool isKeySet(int key) const;
	bool isPressed(int key) const;
	bool isReleased(int key) const;
};

#endif