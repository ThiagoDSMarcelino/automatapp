#include "core/state_set.h"

#include <algorithm>
#include <unordered_set>
#include <utility>

StateSet::StateSet(std::vector<std::shared_ptr<State>> states)
    : m_states(std::move(states)) {}

Result<std::unique_ptr<StateSet>>
StateSet::Create(std::vector<std::shared_ptr<State>> states) {
  std::unordered_set<std::shared_ptr<State>> seen;
  for (const auto &state : states) {
    if (state == nullptr) {
      return std::unexpected("State cannot be NULL");
    }

    if (state->empty()) {
      return std::unexpected("State name cannot be empty");
    }

    if (!seen.insert(state).second) {
      return std::unexpected("Duplicate state in list: " + *state);
    }
  }

  return std::unique_ptr<StateSet>(new StateSet(std::move(states)));
}

bool StateSet::contains(const std::shared_ptr<State> &state) const {
  return std::find(m_states.begin(), m_states.end(), state) != m_states.end();
}
