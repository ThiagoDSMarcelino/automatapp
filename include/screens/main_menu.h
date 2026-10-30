#pragma once

#include "screen_manager.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

class MainMenu : public ftxui::ComponentBase {
public:
  explicit MainMenu(ScreenManager &screen_manager)
      : m_screen_manager(screen_manager) {
    using namespace ftxui;

    MenuOption menu_opt = MenuOption::Vertical();
    menu_opt.on_enter = [this] {
      if (m_selected_option == 3) {
        m_screen_manager.exit();
      } else {
        m_screen_manager.navigate(m_selected_option + 1);
      }
    };

    auto menu = Menu(&m_options, &m_selected_option, menu_opt);

    auto screen = Renderer(menu, [menu] {
      return vbox({
                 text("AUTOMATA++") | bold | center,
                 text("The finite automata workbench") | dim | center,
                 separator(),
                 menu->Render() | center,
             }) |
             border | size(WIDTH, LESS_THAN, 50) | center;
    });

    Add(screen);
  }

private:
  ScreenManager &m_screen_manager;
  std::vector<std::string> m_options = {"Automatas", "Languages", "Tests",
                                        "Exit"};
  int m_selected_option = 0;
};
