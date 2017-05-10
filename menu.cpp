#include "menu.h"

#include "vector.h"

const Vec2f Menu::S_MENU_POS = Vec2f{ 0.2f, 0.25f };
const float Menu::S_MENU_SIZE = 0.001;
const float Menu::S_MENU_OFFSET = 0.05;

const Vec3f Menu::S_DEFAULT_COLOR = Vec3f{ 1.0f, 1.0f, 1.0f };
const Vec3f Menu::S_HOVER_COLOR = Vec3f{ 1.0f, 0.647f, 0.0f };

std::list<Window::TextId>::iterator Menu::findMenuEntry() {
	float relative_x = m_pos[0] / Window::S_WIDTH;
	float relative_y = 1.0f - m_pos[1] / Window::S_HEIGHT;
	
	for (auto&& entry = m_entries.begin(); entry != m_entries.end(); ++entry) {
		if (Window::instance()->isInsideText(*entry, Vec2f{relative_x, relative_y})) {
			return entry;
		}
	}
	
	return m_entries.end();
}