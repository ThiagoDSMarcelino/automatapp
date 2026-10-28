#pragma once

#include "transition.h"

class TransitionD : public Transition {
public:
  TransitionD(std::string symbol, std::shared_ptr<State> from,
              std::shared_ptr<State> to);
};
