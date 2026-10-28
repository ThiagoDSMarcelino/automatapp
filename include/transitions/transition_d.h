#pragma once

#include "core.h"
#include "transition.h"

class TransitionD : public Transition {
public:
  TransitionD(Symbol symbol, std::shared_ptr<State> from,
              std::shared_ptr<State> to);
};
