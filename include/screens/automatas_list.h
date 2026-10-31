#pragma once

#include "core/system_manager.h"
#include "ui/widgets.h"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

class AutomatasList : public ftxui::ComponentBase {
public:
  AutomatasList(const SystemManager &system, const std::string &notice) {
    using namespace ftxui;
    Add(Renderer([&system, &notice] {
      if (system.automatas().empty()) {
        return text("No automata yet. Go to the Create tab.") | dim | center;
      }

      Elements rows;
      for (const auto &automata : system.automatas()) {
        rows.push_back(text("  " + automata->name()));
      }

      rows.push_back(separator());
      rows.push_back(ui::Feedback(notice, false));
      return vbox(std::move(rows));
    }));
  }
};
