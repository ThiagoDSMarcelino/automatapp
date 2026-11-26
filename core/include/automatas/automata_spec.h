#pragma once

#include "automatas/automata_base.h"
#include "automatas/automata_type.h"
#include "core/alphabet.h"
#include "core/language_types.h"
#include <string>
#include <vector>

struct TransitionSpec {
  std::string from;
  Symbol symbol;
  std::vector<std::string> to;
};

struct AutomataSpec {
  std::string name;
  AutomataType type = AutomataType::DFA;
  std::string alphabet;
  std::vector<std::string> states;
  std::string initial;
  std::vector<std::string> finals;
  std::vector<TransitionSpec> transitions;
};

Result<std::shared_ptr<AutomataBase>>
build_automata(const AutomataSpec &spec, std::shared_ptr<Alphabet> alphabet);

AutomataSpec to_spec(const AutomataBase &automata);
