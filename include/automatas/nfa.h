#pragma once

#include "automata.h"
#include "transitions/transition_n.h"

class NFA : public Automata {
public:
  NFA(std::string name, std::unique_ptr<StateSet> states,
      std::shared_ptr<Alphabet> alphabet,
      std::vector<std::shared_ptr<TransitionN>> transitions,
      std::shared_ptr<State> initial_state,
      std::unique_ptr<StateSet> final_states);
};
