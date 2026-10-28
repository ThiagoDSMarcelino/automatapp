#include "transitions/transition_d.h"

TransitionD::TransitionD(std::string symbol, std::shared_ptr<State> from,
                         std::shared_ptr<State> to)
    : Transition(std::move(symbol), std::move(from), {std::move(to)}) {
  if (this->m_symbol.empty()) {
    throw std::invalid_argument(
        "Deterministic transition symbol cannot be empty");
  }
  if (this->m_to.front() == nullptr) {
    throw std::invalid_argument("Transition target state cannot be null");
  }
}
