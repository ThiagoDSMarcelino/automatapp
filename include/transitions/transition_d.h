#pragma once

#include "transitions/transition.h"

class TransitionD : public Transition {
public:
  static Result<std::shared_ptr<TransitionD>>
  Create(Symbol symbol, std::shared_ptr<State> from, std::shared_ptr<State> to);

private:
  TransitionD(Symbol symbol, std::shared_ptr<State> from,
              std::shared_ptr<State> to);
};
