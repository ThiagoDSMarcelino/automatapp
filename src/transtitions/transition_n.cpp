#include "transitions/transition_n.h"

TransitionN::TransitionN(std::string symbol, std::shared_ptr<State> from,
                         std::vector<std::shared_ptr<State>> to)
    : Transition(std::move(symbol), std::move(from), std::move(to)) {
  if (this->m_symbol.empty()) {
    throw std::invalid_argument(
        "Nondeterministic transition symbol cannot be empty");
  }
}
