#include "transitions/transition_d.h"

#include <utility>

TransitionD::TransitionD(Symbol symbol, std::shared_ptr<State> from,
                         std::shared_ptr<State> to)
    : Transition(std::move(symbol), std::move(from), {std::move(to)}) {}

Result<std::shared_ptr<TransitionD>>
TransitionD::Create(Symbol symbol, std::shared_ptr<State> from,
                    std::shared_ptr<State> to) {
  if (auto valid = Validate(symbol, from, {to}); !valid) {
    return std::unexpected(valid.error());
  }

  if (symbol == Epsilon) {
    return std::unexpected("Deterministic transition symbol cannot be " +
                           Epsilon);
  }

  return std::shared_ptr<TransitionD>(
      new TransitionD(std::move(symbol), std::move(from), std::move(to)));
}
