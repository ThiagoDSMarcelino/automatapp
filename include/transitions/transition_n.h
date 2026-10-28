#pragma once

#include "transition.h"

class TransitionN : public Transition {
public:
  TransitionN(Symbol symbol, std::shared_ptr<State> from,
              std::vector<std::shared_ptr<State>> to);
};
