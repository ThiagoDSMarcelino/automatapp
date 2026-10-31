#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class MainMenu : public ftxui::ComponentBase {
public:
  struct Item {
    std::string label;
    std::function<void()> on_select;
  };

  explicit MainMenu(std::vector<Item> items) : m_items(std::move(items)) {
    using namespace ftxui;

    for (const auto &item : m_items) {
      m_labels.push_back(item.label);
    }

    MenuOption menu_opt = MenuOption::Vertical();
    menu_opt.on_enter = [this] { m_items[m_selected_option].on_select(); };

    auto menu = Menu(&m_labels, &m_selected_option, menu_opt);

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
  std::vector<Item> m_items;
  std::vector<std::string> m_labels;
  int m_selected_option = 0;
};
