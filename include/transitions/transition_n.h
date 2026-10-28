#pragma once

#include "transition.h"

class TransitionN : public Transition {
public:
  TransitionN(std::string symbol, std::shared_ptr<State> from,
              std::vector<std::shared_ptr<State>> to);
};
