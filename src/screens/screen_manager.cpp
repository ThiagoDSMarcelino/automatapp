#include "screens/screen_manager.h"
#include "core/system_manager.h"
#include "screens/automatas_screen.h"
#include "screens/main_menu.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <utility>

using namespace ftxui;

Component Placeholder(std::string title, std::function<void()> on_back) {
  auto back = Button("Go back", std::move(on_back));
  return Renderer(back, [=] {
    return vbox({text(title) | bold, separator(), back->Render()}) | border |
           center;
  });
}

ScreenManager::ScreenManager(SystemManager &system) {
  auto go_home = [this] { m_current_screen = m_home_screen; };

  auto open_automatas = add(Make<AutomatasScreen>(system, go_home));
  auto open_languages = add(Placeholder("Languages", go_home));
  auto open_tests = add(Placeholder("Tests", go_home));

  m_home_screen = static_cast<int>(m_screens.size());
  m_current_screen = m_home_screen;
  add(Make<MainMenu>(std::vector<MainMenu::Item>{
      {"Automatas", open_automatas},
      {"Languages", open_languages},
      {"Tests", open_tests},
      {"Exit", [this] { m_screen.Exit(); }},
  }));
}

std::function<void()> ScreenManager::add(Component screen) {
  const int index = static_cast<int>(m_screens.size());
  m_screens.push_back(std::move(screen));
  return [this, index] { m_current_screen = index; };
}

void ScreenManager::run() {
  auto root = Container::Tab(m_screens, &m_current_screen);
  m_screen.Loop(root);
}
