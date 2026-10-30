#include "automatas/enfa.h"

eNFA::eNFA(std::string name, std::unique_ptr<StateSet> states,
           std::shared_ptr<Alphabet> alphabet,
           std::vector<std::shared_ptr<Transition>> transitions,
           std::shared_ptr<State> initial_state,
           std::unique_ptr<StateSet> final_states)
    : Automata(std::move(name), std::move(states), std::move(alphabet),
               std::vector<std::shared_ptr<Transition>>(
                   std::make_move_iterator(transitions.begin()),
                   std::make_move_iterator(transitions.end())),
               std::move(initial_state), std::move(final_states)) {}
