#include "automatas/automata_spec.h"
#include "automatas/dfa.h"
#include "automatas/enfa.h"
#include "automatas/nfa.h"
#include "core/state_set.h"
#include "transitions/transition_d.h"
#include "transitions/transition_n.h"

#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace {

using StateMap = std::unordered_map<std::string, std::shared_ptr<State>>;

Result<std::vector<std::shared_ptr<State>>>
resolve_states(const StateMap &by_name, const std::vector<std::string> &names) {
  std::vector<std::shared_ptr<State>> resolved;
  resolved.reserve(names.size());
  for (const auto &name : names) {
    auto it = by_name.find(name);
    if (it == by_name.end()) {
      return std::unexpected("State '" + name + "' does not exist");
    }

    resolved.push_back(it->second);
  }

  return resolved;
}

std::string transition_label(const TransitionSpec &t) {
  return "δ(" + t.from + ", " + t.symbol + ")";
}

} // namespace

Result<std::shared_ptr<AutomataBase>>
build_automata(const AutomataSpec &spec, std::shared_ptr<Alphabet> alphabet) {
  if (alphabet == nullptr) {
    return std::unexpected("Select an alphabet");
  }

  if (!spec.alphabet.empty() && spec.alphabet != alphabet->name()) {
    return std::unexpected("Alphabet '" + spec.alphabet +
                           "' does not match '" + alphabet->name() + "'");
  }

  if (spec.states.empty()) {
    return std::unexpected("Automata must have at least one state");
  }

  StateMap by_name;
  std::vector<std::shared_ptr<State>> states;
  for (const auto &name : spec.states) {
    if (name.empty()) {
      return std::unexpected("State name cannot be empty");
    }

    auto state = std::make_shared<State>(name);
    if (!by_name.emplace(name, state).second) {
      return std::unexpected("Duplicate state '" + name + "'");
    }

    states.push_back(std::move(state));
  }

  if (spec.initial.empty()) {
    return std::unexpected("Set the initial state");
  }

  auto initial = by_name.find(spec.initial);
  if (initial == by_name.end()) {
    return std::unexpected("Initial state '" + spec.initial +
                           "' does not exist");
  }

  std::unordered_set<std::string> seen_finals;
  for (const auto &name : spec.finals) {
    if (!seen_finals.insert(name).second) {
      return std::unexpected("Duplicate final state '" + name + "'");
    }
  }

  auto finals = resolve_states(by_name, spec.finals);
  if (!finals) {
    return std::unexpected(finals.error());
  }

  auto state_set = StateSet::Create(std::move(states));
  if (!state_set) {
    return std::unexpected(state_set.error());
  }

  auto final_set = StateSet::Create(std::move(*finals));
  if (!final_set) {
    return std::unexpected(final_set.error());
  }

  std::vector<std::shared_ptr<Transition>> transitions;
  for (const auto &t : spec.transitions) {
    auto from = by_name.find(t.from);
    if (from == by_name.end()) {
      return std::unexpected("State '" + t.from + "' does not exist");
    }

    auto to = resolve_states(by_name, t.to);
    if (!to) {
      return std::unexpected(transition_label(t) + ": " + to.error());
    }

    Result<std::shared_ptr<Transition>> transition;
    switch (spec.type) {
    case AutomataType::DFA:
      if (to->size() != 1) {
        return std::unexpected(transition_label(t) + " must have exactly one target");
      }
      transition = TransitionD::Create(t.symbol, from->second, to->front());
      break;
    case AutomataType::NFA:
      transition = TransitionN::Create(t.symbol, from->second, *to);
      break;
    case AutomataType::eNFA:
      transition = Transition::Create(t.symbol, from->second, *to);
      break;
    }

    if (!transition) {
      return std::unexpected(transition_label(t) + ": " + transition.error());
    }

    transitions.push_back(std::move(*transition));
  }

  const auto downcast = [&]<typename T>() {
    std::vector<std::shared_ptr<T>> typed;
    typed.reserve(transitions.size());
    for (const auto &t : transitions) {
      typed.push_back(std::static_pointer_cast<T>(t));
    }
    return typed;
  };

  Result<std::shared_ptr<AutomataBase>> built =
      std::unexpected("Unknown automaton type");
  switch (spec.type) {
  case AutomataType::DFA:
    built = DFA::Create(spec.name, std::move(*state_set), std::move(alphabet),
                        downcast.template operator()<TransitionD>(),
                        initial->second, std::move(*final_set));
    break;
  case AutomataType::NFA:
    built = NFA::Create(spec.name, std::move(*state_set), std::move(alphabet),
                        downcast.template operator()<TransitionN>(),
                        initial->second, std::move(*final_set));
    break;
  case AutomataType::eNFA:
    built = eNFA::Create(spec.name, std::move(*state_set),
                         std::move(alphabet), std::move(transitions),
                         initial->second, std::move(*final_set));
    break;
  }

  return built;
}

AutomataSpec to_spec(const AutomataBase &automata) {
  AutomataSpec spec;
  spec.name = automata.name();
  spec.type = automata.type();
  spec.alphabet = automata.alphabet()->name();
  spec.initial = *automata.initial_state();

  for (const auto &state : automata.states()) {
    spec.states.push_back(*state);
  }

  for (const auto &state : automata.final_states()) {
    spec.finals.push_back(*state);
  }

  for (const auto &t : automata.transitions()) {
    TransitionSpec ts{*t->from(), t->symbol(), {}};
    for (const auto &state : t->to()) {
      ts.to.push_back(*state);
    }
    spec.transitions.push_back(std::move(ts));
  }

  return spec;
}
