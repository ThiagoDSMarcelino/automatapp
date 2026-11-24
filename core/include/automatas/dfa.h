#pragma once

#include "automatas/automata_base.h"
#include "transitions/transition_d.h"

class DFA : public AutomataBase {
public:
  static Result<std::shared_ptr<DFA>>
  Create(std::string name, std::unique_ptr<StateSet> states,
         std::shared_ptr<Alphabet> alphabet,
         std::vector<std::shared_ptr<TransitionD>> transitions,
         std::shared_ptr<State> initial_state,
         std::unique_ptr<StateSet> final_states);

private:
  DFA(std::string name, std::unique_ptr<StateSet> states,
      std::shared_ptr<Alphabet> alphabet,
      std::vector<std::shared_ptr<Transition>> transitions,
      std::shared_ptr<State> initial_state,
      std::unique_ptr<StateSet> final_states);
};
