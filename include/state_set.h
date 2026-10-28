#pragma once

#include "state.h"
#include <memory>
#include <vector>

class StateSet {
private:
  std::vector<std::shared_ptr<State>> m_states;

public:
  explicit StateSet(std::vector<std::shared_ptr<State>> states);
  bool contains(const std::shared_ptr<State> &state) const;

  auto begin() const { return m_states.cbegin(); }
  auto end() const { return m_states.cend(); }
};
