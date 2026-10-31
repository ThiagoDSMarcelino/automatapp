#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace ui {

inline ftxui::Element FieldLabel(const std::string &label, bool focused) {
  using namespace ftxui;
  auto element = text((focused ? "> " : "  ") + label) | size(WIDTH, EQUAL, 16);
  return focused ? element | bold | color(Color::Cyan) : element;
}

inline ftxui::Element Field(const std::string &label, ftxui::Component input) {
  using namespace ftxui;
  return hbox({
      FieldLabel(label, input->Focused()),
      input->Render() | flex,
  });
}

inline ftxui::Element Feedback(const std::string &message, bool is_error) {
  using namespace ftxui;
  if (message.empty()) {
    return text("");
  }
  return text(message) | color(is_error ? Color::Red : Color::Green);
}

inline ftxui::InputOption InputStyle(const std::string &placeholder) {
  using namespace ftxui;
  InputOption option;
  option.placeholder = placeholder;
  option.multiline = false;
  option.transform = [](InputState state) {
    state.element |= bgcolor(state.focused ? Color::Blue : Color::GrayDark);
    if (state.is_placeholder) {
      state.element |= dim;
    }
    if (state.focused) {
      state.element |= color(Color::White);
    }
    return state.element;
  };
  return option;
}

inline ftxui::Component ListInput(std::string *buffer,
                                  const std::string &placeholder,
                                  std::function<void()> on_add,
                                  std::function<bool()> on_remove_last) {
  using namespace ftxui;
  auto option = InputStyle(placeholder);
  option.on_enter = std::move(on_add);
  return Input(buffer, option) | CatchEvent([buffer, on_remove_last](Event e) {
           if (e == Event::Backspace && buffer->empty()) {
             return on_remove_last();
           }
           return false;
         });
}

inline ftxui::Element Chips(const std::vector<std::string> &items,
                            const std::string &empty_hint) {
  using namespace ftxui;
  if (items.empty()) {
    return text(empty_hint) | dim;
  }
  Elements chips;
  for (const auto &item : items) {
    chips.push_back(text(" " + item + " ") | inverted);
    chips.push_back(text(" "));
  }
  return hflow(std::move(chips));
}

inline ftxui::Component Section(const std::string &title,
                                ftxui::Component list_tab,
                                ftxui::Component create_tab, int *active_tab,
                                std::function<void()> on_back) {
  using namespace ftxui;
  static const std::vector<std::string> tab_names = {" List ", " Create "};

  auto tabs = Menu(&tab_names, active_tab, MenuOption::Toggle());
  auto content = Container::Tab({list_tab, create_tab}, active_tab);
  auto back_button =
      Button("<- Back", std::move(on_back), ButtonOption::Ascii());

  auto layout = Container::Vertical({tabs, content, back_button});

  return Renderer(layout, [=] {
    return window(text(" " + title + " ") | bold,
                  vbox({
                      tabs->Render(),
                      separator(),
                      content->Render() | flex,
                      separator(),
                      hbox({
                          back_button->Render(),
                          filler(),
                          text("Tab/arrows: move  Enter: confirm ") | dim,
                      }),
                  }));
  });
}

} // namespace ui
