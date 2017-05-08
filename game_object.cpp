#include "game_object.h"
#include "engine.h"
#include "debug.h"

void GameObject::update() {
	if (m_view != nullptr)
		m_view->draw(&m_engine->getWindow(), m_pos, m_front, m_up, m_size);
	m_logic->logic();
}