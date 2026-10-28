#pragma once

#include "automata.h"
#include "transitions/transition_d.h"

class DFA : public Automata {
public:
  DFA(std::string name, std::unique_ptr<States> states,
      std::shared_ptr<Alphabet> alphabet,
      std::vector<std::shared_ptr<TransitionD>> transitions,
      std::shared_ptr<State> initial_state,
      std::unique_ptr<States> final_states);
};
