#include "transitions/transition_n.h"

#include <utility>

TransitionN::TransitionN(Symbol symbol, std::shared_ptr<State> from,
                         std::vector<std::shared_ptr<State>> to)
    : Transition(std::move(symbol), std::move(from), std::move(to)) {}

Result<std::shared_ptr<TransitionN>>
TransitionN::Create(Symbol symbol, std::shared_ptr<State> from,
                    std::vector<std::shared_ptr<State>> to) {
  if (auto valid = Validate(symbol, from, to); !valid) {
    return std::unexpected(valid.error());
  }

  if (symbol == Epsilon) {
    return std::unexpected("Nondeterministic transition symbol cannot be " +
                           Epsilon);
  }

  return std::shared_ptr<TransitionN>(
      new TransitionN(std::move(symbol), std::move(from), std::move(to)));
}
