#ifndef MENU_H
#define MENU_H

#include "mouse_event.h"
#include "input_event.h"

#include "listener.h"
#include "notifier.h"

#include "input_notifier.h"
#include "mouse_notifier.h"

#include "window.h"

struct MenuEvent {
	int m_entry_num = 0;
};

class Menu : public Listener<MouseEvent>, public Listener<InputEvent>, public Notifier<MenuEvent> {
public:
	
	Menu() {
		InputNotifier::instance()->subscribe(this);
		MouseNotifier::instance()->subscribe(this);
	}
	
	void addEntry(const std::string& entry_name) {
		Vec2f current_pos = S_MENU_POS;
		current_pos[1] -= S_MENU_OFFSET * m_entries.size();
		Window::TextId id = Window::instance()->registerText(entry_name, current_pos, S_MENU_SIZE);
		if (m_disabled) {
			Window::instance()->hideText(id);
		}
		m_entries.push_back(id);
	}
	
	void disable() {
		for (auto&& entry : m_entries) {
			Window::instance()->hideText(entry);
		}
		m_disabled = true;
	}
	
	void enable() {
		for (auto&& entry : m_entries) {
			Window::instance()->showText(entry);
		}
		m_disabled = false;
	}
	
	virtual void onEvent(MouseEvent event) {
		m_pos = Vec2f{event.xpos, event.ypos};
		auto me = findMenuEntry();
		for (auto&& entry : m_entries) {
			Window::instance()->setTextColor(entry, Vec3f{1.0, 1.0, 1.0});
		}
		if (me != m_entries.end()) {
			Window::instance()->setTextColor(*me, Vec3f{1.0, 0.0, 0.0});
		}
	}
	
	virtual void onEvent(InputEvent event) {
		if (m_disabled) return;
		
		if (event == InputEvent::MOUSE_LEFT) {
			auto me = findMenuEntry();
			if (me != m_entries.end()) {
				notify(MenuEvent{static_cast<int>(std::distance(m_entries.begin(), me))});
			}
		}
	}
	
	virtual ~Menu() {
		for (auto&& entry : m_entries) {
			Window::instance()->destroyText(entry);
		}
		m_entries.clear();
		
		InputNotifier::instance()->unsubscribe(this);
		MouseNotifier::instance()->unsubscribe(this);
	}
	
private:

	std::list<Window::TextId>::iterator findMenuEntry();
	
	static const Vec2f S_MENU_POS;
	static const float S_MENU_SIZE;
	static const float S_MENU_OFFSET;
	
	static const Vec3f S_DEFAULT_COLOR;
	static const Vec3f S_HOVER_COLOR;
	
	bool m_disabled = true;
	
	Vec2f m_pos = Vec2f{0, 0};
	
	std::list<Window::TextId> m_entries;
};

#endif