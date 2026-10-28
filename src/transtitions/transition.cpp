#include "transitions/transition.h"

#include <cstring>
#include <stdexcept>
#include <utility>

Transition::Transition(std::string symbol, std::shared_ptr<State> from,
                       std::vector<std::shared_ptr<State>> to)
    : m_symbol(std::move(symbol)), m_from(std::move(from)),
      m_to(std::move(to)) {
  if (this->m_from == nullptr) {
    throw std::invalid_argument("Transition from state cannot be NULL");
  }
}
