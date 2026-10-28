#pragma once

#include "alphabet.h"
#include "core.h"
#include "state.h"
#include "state_set.h"
#include "transitions/transition.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Automata {
private:
  std::unordered_map<std::shared_ptr<State>,
                     std::vector<std::shared_ptr<Transition>>>
      m_transitions_by_state;

protected:
  Automata(std::string name, std::unique_ptr<StateSet> states,
           std::shared_ptr<Alphabet> alphabet,
           std::vector<std::shared_ptr<Transition>> transitions,
           std::shared_ptr<State> initial_state,
           std::unique_ptr<StateSet> final_states);

  std::string m_name;
  std::unique_ptr<StateSet> m_states;
  std::shared_ptr<Alphabet> m_alphabet;
  std::vector<std::shared_ptr<Transition>> m_transitions;
  std::shared_ptr<State> m_initial_state;
  std::unique_ptr<StateSet> m_final_states;

public:
  const std::string &name() const { return m_name; };
  bool accept(const Word &word) const;
  virtual ~Automata() = default;
};
