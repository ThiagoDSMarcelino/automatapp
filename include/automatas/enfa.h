#pragma once

#include "automata.h"

class eNFA : public Automata {
public:
  eNFA(std::string name, std::unique_ptr<StateSet> states,
       std::shared_ptr<Alphabet> alphabet,
       std::vector<std::shared_ptr<Transition>> transitions,
       std::shared_ptr<State> initial_state,
       std::unique_ptr<StateSet> final_states);
};
