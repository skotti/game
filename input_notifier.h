#ifndef INPUT_NOTIFIER
#define INPUT_NOTIFIER

#include "debug.h"
#include "stl_headers.h"
#include "game_object.h"
#include "notifier.h"
#include "input_event.h"
#include "logger.h"

class InputNotifier : public Notifier<InputEvent> {
public:
	InputNotifier() {
		m_logger.setPrefix("InputNotifier:: ");
		m_logger.log("Create InputNotifier");
	}
	void input();
	
private:
	bool isKeySet(int key) const {
		ASSERT(0 <= key && key < S_BUF_SIZE);
		m_logger.log("Key % is pressed", key);
		return m_keys[key]; 
	}
	
	static const int S_BUF_SIZE = 1024;
	
	std::array<bool, S_BUF_SIZE> m_keys;
	double m_cursor_x;
	double m_cursor_y;	
	
	Logger m_logger;
};

#endif