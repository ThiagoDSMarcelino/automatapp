#pragma once

#include "core.h"
#include "state.h"
#include <memory>
#include <string>
#include <vector>

class Transition {
protected:
  std::string m_symbol;
  std::shared_ptr<State> m_from;
  std::vector<std::shared_ptr<State>> m_to;

public:
  Transition(Symbol symbol, std::shared_ptr<State> from,
             std::vector<std::shared_ptr<State>> to);
  virtual ~Transition() = default;

  const Symbol &symbol() const { return m_symbol; }
  const std::shared_ptr<State> &from() const { return m_from; }
  const std::vector<std::shared_ptr<State>> &to() const { return m_to; }
};
