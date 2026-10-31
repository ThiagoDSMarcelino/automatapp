#pragma once

#include "automatas/automata_draft.h"
#include "core/system_manager.h"

#include <ftxui/component/component.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class AutomataForm : public ftxui::ComponentBase {
public:
  AutomataForm(const SystemManager &system,
               std::function<void(std::shared_ptr<AutomataBase>)> on_saved);

private:
  void Apply(const Result<void> &result, std::string &buffer);
  void ShowError(std::string message);
  void OnSave();
  void ClearForm();

  void RefreshAlphabets();
  bool HasAlphabets() const;
  std::shared_ptr<Alphabet> SelectedAlphabet() const;

  const SystemManager &m_system;
  std::function<void(std::shared_ptr<AutomataBase>)> m_on_saved;

  AutomataDraft m_draft;
  std::vector<std::string> m_types = {"DFA", "NFA", "eNFA"};

  std::string m_state_buffer;
  std::string m_final_buffer;
  std::string m_transition_buffer;

  std::vector<std::string> m_alphabet_labels;
  int m_selected_alphabet = 0;

  std::string m_message;
  bool m_is_error = false;
};
