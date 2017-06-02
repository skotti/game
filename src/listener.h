#ifndef LISTENER_H
#define LISTENER_H

template <typename EventType>
class Listener {
public:
	virtual void onEvent(EventType event) = 0;
	virtual ~Listener(){}
};

#endif