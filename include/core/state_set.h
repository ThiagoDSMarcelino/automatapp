#pragma once

#include "core/language_types.h"
#include <memory>
#include <vector>

class StateSet {
public:
  static Result<std::unique_ptr<StateSet>>
  Create(std::vector<std::shared_ptr<State>> states);

  bool contains(const std::shared_ptr<State> &state) const;

  auto begin() const { return m_states.cbegin(); }
  auto end() const { return m_states.cend(); }

private:
  std::vector<std::shared_ptr<State>> m_states;

  explicit StateSet(std::vector<std::shared_ptr<State>> states);
};
