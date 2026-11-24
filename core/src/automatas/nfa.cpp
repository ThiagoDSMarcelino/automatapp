#include "automatas/nfa.h"

#include <utility>

NFA::NFA(std::string name, std::unique_ptr<StateSet> states,
         std::shared_ptr<Alphabet> alphabet,
         std::vector<std::shared_ptr<Transition>> transitions,
         std::shared_ptr<State> initial_state,
         std::unique_ptr<StateSet> final_states)
    : AutomataBase(std::move(name), std::move(states), std::move(alphabet),
                   std::move(transitions), std::move(initial_state),
                   std::move(final_states)) {}

Result<std::shared_ptr<NFA>>
NFA::Create(std::string name, std::unique_ptr<StateSet> states,
            std::shared_ptr<Alphabet> alphabet,
            std::vector<std::shared_ptr<TransitionN>> transitions,
            std::shared_ptr<State> initial_state,
            std::unique_ptr<StateSet> final_states) {
  std::vector<std::shared_ptr<Transition>> base_transitions(
      std::make_move_iterator(transitions.begin()),
      std::make_move_iterator(transitions.end()));

  auto valid = Validate(name, states, alphabet, base_transitions, initial_state,
                        final_states);
  if (!valid) {
    return std::unexpected(valid.error());
  }

  return std::shared_ptr<NFA>(
      new NFA(std::move(name), std::move(states), std::move(alphabet),
              std::move(base_transitions), std::move(initial_state),
              std::move(final_states)));
}
