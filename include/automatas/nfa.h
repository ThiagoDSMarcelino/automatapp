#pragma once

#include "automatas/automata_base.h"
#include "transitions/transition_n.h"

class NFA : public AutomataBase {
public:
  static Result<std::shared_ptr<NFA>>
  Create(std::string name, std::unique_ptr<StateSet> states,
         std::shared_ptr<Alphabet> alphabet,
         std::vector<std::shared_ptr<TransitionN>> transitions,
         std::shared_ptr<State> initial_state,
         std::unique_ptr<StateSet> final_states);

private:
  NFA(std::string name, std::unique_ptr<StateSet> states,
      std::shared_ptr<Alphabet> alphabet,
      std::vector<std::shared_ptr<Transition>> transitions,
      std::shared_ptr<State> initial_state,
      std::unique_ptr<StateSet> final_states);
};
