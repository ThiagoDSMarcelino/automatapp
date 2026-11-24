#pragma once

#include "automatas/automata_type.h"
#include "core/alphabet.h"
#include "core/language_types.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class AutomataBase;

class AutomataDraft {
public:
  std::string name;
  std::string initial;
  int type_index = static_cast<int>(AutomataType::DFA);

  Result<void> add_state(const std::string &state);
  bool remove_last_state();

  Result<void> add_final(const std::string &state);
  bool remove_last_final();

  Result<void> add_transition(const std::string &line,
                              const Alphabet *alphabet);
  bool remove_last_transition();

  Result<std::shared_ptr<AutomataBase>>
  build(std::shared_ptr<Alphabet> alphabet) const;

  const std::vector<std::string> &states() const { return m_states; }
  const std::vector<std::string> &finals() const { return m_finals; }
  std::vector<std::string> transition_labels() const;

private:
  struct TransitionDraft {
    std::string from;
    Symbol symbol;
    std::vector<std::string> to;
  };

  using StateMap = std::unordered_map<std::string, std::shared_ptr<State>>;

  AutomataType type() const { return static_cast<AutomataType>(type_index); }
  bool is_state(const std::string &state) const;
  Result<void> check_symbol(const Symbol &symbol,
                            const Alphabet *alphabet) const;

  std::vector<std::string> m_states;
  std::vector<std::string> m_finals;
  std::vector<TransitionDraft> m_transitions;
};
