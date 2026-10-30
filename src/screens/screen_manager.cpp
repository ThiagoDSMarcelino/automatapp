#include "screens/screen_manager.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

void ScreenManager::add(Component screen) { m_screens.push_back(screen); }

void ScreenManager::navigate(int screen_index) {
  if (screen_index >= 0 && screen_index < (int)m_screens.size()) {
    m_current_screen = screen_index;
  }
}

void ScreenManager::exit() { m_screen.Exit(); }

void ScreenManager::run() {
  auto root = Container::Tab(m_screens, &m_current_screen);
  m_screen.Loop(root);
}
