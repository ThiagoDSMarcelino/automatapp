#include "state.h"

#include <stdexcept>
#include <utility>

State::State(std::string name) : m_name(std::move(name)) {
  if (this->m_name.empty()) {
    throw std::invalid_argument("State name cannot be empty");
  }
}
