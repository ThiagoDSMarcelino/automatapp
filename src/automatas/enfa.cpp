#include "automatas/enfa.h"

#include <utility>

eNFA::eNFA(std::string name, std::unique_ptr<StateSet> states,
           std::shared_ptr<Alphabet> alphabet,
           std::vector<std::shared_ptr<Transition>> transitions,
           std::shared_ptr<State> initial_state,
           std::unique_ptr<StateSet> final_states)
    : AutomataBase(std::move(name), std::move(states), std::move(alphabet),
                   std::move(transitions), std::move(initial_state),
                   std::move(final_states)) {}

Result<std::shared_ptr<eNFA>>
eNFA::Create(std::string name, std::unique_ptr<StateSet> states,
             std::shared_ptr<Alphabet> alphabet,
             std::vector<std::shared_ptr<Transition>> transitions,
             std::shared_ptr<State> initial_state,
             std::unique_ptr<StateSet> final_states) {
  auto valid = Validate(name, states, alphabet, transitions, initial_state,
                        final_states);
  if (!valid) {
    return std::unexpected(valid.error());
  }

  return std::shared_ptr<eNFA>(
      new eNFA(std::move(name), std::move(states), std::move(alphabet),
               std::move(transitions), std::move(initial_state),
               std::move(final_states)));
}
