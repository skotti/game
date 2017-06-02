#include "game_object.h"
#include "engine.h"
#include "debug.h"

void GameObject::update() {
	m_logic->logic();
	m_graphic->update();
}