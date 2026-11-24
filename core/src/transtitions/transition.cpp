#include "transitions/transition.h"

#include <utility>

Transition::Transition(Symbol symbol, std::shared_ptr<State> from,
                       std::vector<std::shared_ptr<State>> to)
    : m_symbol(std::move(symbol)), m_from(std::move(from)),
      m_to(std::move(to)) {}

Result<void>
Transition::Validate(const Symbol &symbol, const std::shared_ptr<State> &from,
                     const std::vector<std::shared_ptr<State>> &to) {
  if (symbol.empty()) {
    return std::unexpected("Transition symbol cannot be empty");
  }

  if (from == nullptr) {
    return std::unexpected("Transition from state cannot be NULL");
  }

  if (to.empty()) {
    return std::unexpected("Transition must have at least one target state");
  }

  for (const auto &state : to) {
    if (state == nullptr) {
      return std::unexpected("Transition target state cannot be NULL");
    }
  }

  return {};
}

Result<std::shared_ptr<Transition>>
Transition::Create(Symbol symbol, std::shared_ptr<State> from,
                   std::vector<std::shared_ptr<State>> to) {
  if (auto valid = Validate(symbol, from, to); !valid) {
    return std::unexpected(valid.error());
  }

  return std::shared_ptr<Transition>(
      new Transition(std::move(symbol), std::move(from), std::move(to)));
}
