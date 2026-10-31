#pragma once

#include "core/system_manager.h"
#include "screens/automata_form.h"
#include "screens/automatas_list.h"
#include "ui/widgets.h"

#include <ftxui/component/component.hpp>
#include <functional>
#include <memory>
#include <string>
#include <utility>

class AutomatasScreen : public ftxui::ComponentBase {
public:
  AutomatasScreen(SystemManager &system, std::function<void()> on_back) {
    auto list_tab = ftxui::Make<AutomatasList>(system, m_notice);
    auto create_tab = ftxui::Make<AutomataForm>(
        system, [this, &system](std::shared_ptr<AutomataBase> automata) {
          m_notice = "Automaton '" + automata->name() + "' saved!";
          system.add_automata(std::move(automata));
          m_active_tab = 0;
        });

    Add(ui::Section("Automatas", list_tab, create_tab, &m_active_tab,
                    [this, on_back = std::move(on_back)] {
                      m_notice.clear();
                      on_back();
                    }));
  }

private:
  int m_active_tab = 0;
  std::string m_notice;
};
