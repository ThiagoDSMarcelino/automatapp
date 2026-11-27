#pragma once

#include <optional>
#include <string_view>

enum class AutomataType {
  DFA = 0,
  NFA = 1,
  eNFA = 2,
};

constexpr std::string_view automata_type_name(AutomataType type) {
  switch (type) {
  case AutomataType::DFA:
    return "DFA";
  case AutomataType::NFA:
    return "NFA";
  case AutomataType::eNFA:
    return "eNFA";
  }

  return "";
}

constexpr std::optional<AutomataType>
parse_automata_type(std::string_view name) {
  for (auto type : {AutomataType::DFA, AutomataType::NFA, AutomataType::eNFA}) {
    if (automata_type_name(type) == name) {
      return type;
    }
  }

  return std::nullopt;
}
