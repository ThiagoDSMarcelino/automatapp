#include "automatas/dfa.h"
#include <set>

DFA::DFA(std::string name, std::unique_ptr<StateSet> states,
         std::shared_ptr<Alphabet> alphabet,
         std::vector<std::shared_ptr<TransitionD>> transitions,
         std::shared_ptr<State> initial_state,
         std::unique_ptr<StateSet> final_states)
    : Automata(std::move(name), std::move(states), std::move(alphabet),
               std::vector<std::shared_ptr<Transition>>(
                   std::make_move_iterator(transitions.begin()),
                   std::make_move_iterator(transitions.end())),
               std::move(initial_state), std::move(final_states)) {
  std::set<std::pair<std::shared_ptr<State>, std::string>> seen;
  for (const auto &transition : this->m_transitions) {
    auto pair = std::make_pair(transition->from(), transition->symbol());

    if (!seen.insert(pair).second) {
      throw std::invalid_argument("Duplicate transition from state " +
                                  transition->from()->name() + " for symbol " +
                                  transition->symbol());
    }
  }
}
