#include "game_graphic.h"

#include "game_object.h"

void ModelGraphic::update()
{
	ASSERT(m_id != -1);
	ASSERT(m_game_object != nullptr);
	Window::instance()->setPos(m_id, m_game_object->getPos());
	Window::instance()->setFront(m_id, m_game_object->getFront());
	Window::instance()->setUp(m_id, m_game_object->getUp());
	Window::instance()->setSize(m_id, m_game_object->getSize());
}