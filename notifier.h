#ifndef NOTIFIER_H
#define NOTIFIER_H

#include "listener.h"
#include "stl_headers.h"
#include "debug.h"

template <typename EventType>
class Notifier {
public:
	void subscribe(Listener<EventType>* listener) {
		auto pair = m_listeners.insert(listener);
		DASSERT(pair.second, "Already inserted");
	}
	void unsubscribe(Listener<EventType>* listener) {
		auto it = m_listeners.find(listener);
		DASSERT(it != m_listeners.end(), "Not present");
		m_listeners.erase(it);
	}
	void notify(EventType event) {
		for (auto&& listener : m_listeners) {
			listener->onEvent(event);
		}
	}
	
	virtual ~Notifier() {
		ASSERT(m_listeners.empty());
	}
private:
	std::set<Listener<EventType>*> m_listeners;
};

#endif