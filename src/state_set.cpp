#include "state_set.h"
#include <algorithm>
#include <unordered_set>

StateSet::StateSet(std::vector<std::shared_ptr<State>> states)
    : m_states(std::move(states)) {
  std::unordered_set<std::shared_ptr<State>> seen;
  for (const auto &state : this->m_states) {
    if (state == nullptr) {
      throw std::invalid_argument("State cannot be NULL");
    }

    if (!seen.insert(state).second) {
      throw std::invalid_argument("Duplicate state in list: " + state->name());
    }
  }
}

bool StateSet::contains(const std::shared_ptr<State> &state) const {
  return std::find(m_states.begin(), m_states.end(), state) != m_states.end();
}
