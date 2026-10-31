#pragma once

#include "transitions/transition.h"

class TransitionN : public Transition {
public:
  static Result<std::shared_ptr<TransitionN>>
  Create(Symbol symbol, std::shared_ptr<State> from,
         std::vector<std::shared_ptr<State>> to);

private:
  TransitionN(Symbol symbol, std::shared_ptr<State> from,
              std::vector<std::shared_ptr<State>> to);
};
