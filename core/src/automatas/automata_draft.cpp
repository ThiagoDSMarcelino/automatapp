#include "automatas/automata_draft.h"
#include "automatas/dfa.h"
#include "automatas/enfa.h"
#include "automatas/nfa.h"
#include "transitions/transition_d.h"
#include "transitions/transition_n.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

namespace {

std::string Trim(const std::string &s) {
  const auto first = s.find_first_not_of(" \t");
  if (first == std::string::npos) {
    return "";
  }

  const auto last = s.find_last_not_of(" \t");
  return s.substr(first, last - first + 1);
}

bool Contains(const std::vector<std::string> &v, const std::string &x) {
  return std::find(v.begin(), v.end(), x) != v.end();
}

bool IsEpsilonInput(const std::string &s) {
  return s == "eps" || s == "epsilon" || s == Epsilon;
}

} // namespace

bool AutomataDraft::is_state(const std::string &state) const {
  return Contains(m_states, state);
}

Result<void> AutomataDraft::add_state(const std::string &state) {
  const std::string trimmed = Trim(state);
  if (trimmed.empty()) {
    return {};
  }

  if (is_state(trimmed)) {
    return std::unexpected("State '" + trimmed + "' already exists.");
  }

  m_states.push_back(trimmed);
  return {};
}

bool AutomataDraft::remove_last_state() {
  if (m_states.empty()) {
    return false;
  }

  const std::string removed = m_states.back();
  m_states.pop_back();

  m_finals.erase(std::remove(m_finals.begin(), m_finals.end(), removed),
                 m_finals.end());

  for (auto &t : m_transitions) {
    t.to.erase(std::remove(t.to.begin(), t.to.end(), removed), t.to.end());
  }

  m_transitions.erase(std::remove_if(m_transitions.begin(), m_transitions.end(),
                                     [&](const TransitionDraft &t) {
                                       return t.from == removed || t.to.empty();
                                     }),
                      m_transitions.end());
  return true;
}

Result<void> AutomataDraft::add_final(const std::string &state) {
  const std::string trimmed = Trim(state);
  if (trimmed.empty()) {
    return {};
  }

  if (!is_state(trimmed)) {
    return std::unexpected("State '" + trimmed +
                           "' does not exist. Add it in States.");
  }

  if (Contains(m_finals, trimmed)) {
    return std::unexpected("'" + trimmed + "' is already final.");
  }

  m_finals.push_back(trimmed);
  return {};
}

bool AutomataDraft::remove_last_final() {
  if (m_finals.empty()) {
    return false;
  }

  m_finals.pop_back();
  return true;
}

Result<void> AutomataDraft::check_symbol(const Symbol &symbol,
                                         const Alphabet *alphabet) const {
  if (symbol != Epsilon) {
    if (alphabet == nullptr) {
      return std::unexpected("Select an alphabet first.");
    }

    if (!alphabet->contains(symbol)) {
      return std::unexpected("Symbol '" + symbol + "' is not in alphabet '" +
                             alphabet->name() + "'.");
    }

    return {};
  }

  auto probe = std::make_shared<State>("probe");
  if (type() == AutomataType::DFA) {
    auto transition = TransitionD::Create(symbol, probe, probe);
    if (!transition) {
      return std::unexpected(transition.error());
    }
  } else if (type() == AutomataType::NFA) {
    auto transition = TransitionN::Create(symbol, probe, {probe});
    if (!transition) {
      return std::unexpected(transition.error());
    }
  }

  return {};
}

Result<void> AutomataDraft::add_transition(const std::string &line,
                                           const Alphabet *alphabet) {
  std::string normalized = line;
  std::replace(normalized.begin(), normalized.end(), ',', ' ');
  std::istringstream in(normalized);
  std::vector<std::string> tokens;
  for (std::string tok; in >> tok;) {
    tokens.push_back(tok);
  }

  if (tokens.empty()) {
    return {};
  }

  if (tokens.size() < 3) {
    return std::unexpected("Use: <source> <symbol> <target>");
  }

  const std::string from = tokens[0];
  const Symbol symbol = IsEpsilonInput(tokens[1]) ? Epsilon : tokens[1];

  std::vector<std::string> targets;
  for (size_t i = 2; i < tokens.size(); ++i) {
    if (!Contains(targets, tokens[i])) {
      targets.push_back(tokens[i]);
    }
  }

  if (!is_state(from)) {
    return std::unexpected("State '" + from + "' does not exist.");
  }

  for (const auto &t : targets) {
    if (!is_state(t)) {
      return std::unexpected("State '" + t + "' does not exist.");
    }
  }

  auto valid = check_symbol(symbol, alphabet);
  if (!valid) {
    return valid;
  }

  if (type() == AutomataType::DFA && targets.size() > 1) {
    return std::unexpected("A DFA transition has exactly one target.");
  }

  // Same (from, symbol) already typed: DFA rejects, NFA merges targets.
  auto it = std::find_if(m_transitions.begin(), m_transitions.end(),
                         [&](const TransitionDraft &t) {
                           return t.from == from && t.symbol == symbol;
                         });
  if (it == m_transitions.end()) {
    m_transitions.push_back({from, symbol, targets});
    return {};
  }

  if (type() == AutomataType::DFA) {
    return std::unexpected("δ(" + from + ", " + symbol +
                           ") is already defined.");
  }

  for (const auto &t : targets) {
    if (!Contains(it->to, t)) {
      it->to.push_back(t);
    }
  }

  return {};
}

bool AutomataDraft::remove_last_transition() {
  if (m_transitions.empty()) {
    return false;
  }

  m_transitions.pop_back();
  return true;
}

Result<std::shared_ptr<AutomataBase>>
AutomataDraft::build(std::shared_ptr<Alphabet> alphabet) const {
  if (alphabet == nullptr) {
    return std::unexpected("Create an alphabet first.");
  }

  const std::string trimmed_initial = Trim(initial);
  if (trimmed_initial.empty()) {
    return std::unexpected("Set the initial state.");
  }

  if (!is_state(trimmed_initial)) {
    return std::unexpected("Initial state '" + trimmed_initial +
                           "' does not exist.");
  }

  StateMap by_name;
  std::vector<std::shared_ptr<State>> states;
  for (const auto &state : m_states) {
    by_name[state] = std::make_shared<State>(state);
    states.push_back(by_name[state]);
  }

  std::vector<std::shared_ptr<State>> finals;
  for (const auto &state : m_finals) {
    finals.push_back(by_name.at(state));
  }

  auto state_set = StateSet::Create(std::move(states));
  if (!state_set) {
    return std::unexpected(state_set.error());
  }

  auto final_set = StateSet::Create(std::move(finals));
  if (!final_set) {
    return std::unexpected(final_set.error());
  }

  const auto initial_state = by_name.at(trimmed_initial);
  const auto targets_of = [&](const TransitionDraft &t) {
    std::vector<std::shared_ptr<State>> to;
    for (const auto &state : t.to) {
      to.push_back(by_name.at(state));
    }

    return to;
  };

  switch (type()) {
  case AutomataType::DFA: {
    std::vector<std::shared_ptr<TransitionD>> transitions;
    for (const auto &t : m_transitions) {
      if (t.to.size() != 1) {
        return std::unexpected("DFA: δ(" + t.from + ", " + t.symbol +
                               ") must have exactly one target.");
      }

      auto transition = TransitionD::Create(t.symbol, by_name.at(t.from),
                                            by_name.at(t.to.front()));
      if (!transition) {
        return std::unexpected(transition.error());
      }

      transitions.push_back(*transition);
    }

    return DFA::Create(name, std::move(*state_set), std::move(alphabet),
                       std::move(transitions), initial_state,
                       std::move(*final_set));
  }
  case AutomataType::NFA: {
    std::vector<std::shared_ptr<TransitionN>> transitions;
    for (const auto &t : m_transitions) {
      auto transition =
          TransitionN::Create(t.symbol, by_name.at(t.from), targets_of(t));
      if (!transition) {
        return std::unexpected(transition.error());
      }

      transitions.push_back(*transition);
    }

    return NFA::Create(name, std::move(*state_set), std::move(alphabet),
                       std::move(transitions), initial_state,
                       std::move(*final_set));
  }
  case AutomataType::eNFA: {
    std::vector<std::shared_ptr<Transition>> transitions;
    for (const auto &t : m_transitions) {
      auto transition =
          Transition::Create(t.symbol, by_name.at(t.from), targets_of(t));
      if (!transition) {
        return std::unexpected(transition.error());
      }

      transitions.push_back(*transition);
    }

    return eNFA::Create(name, std::move(*state_set), std::move(alphabet),
                        std::move(transitions), initial_state,
                        std::move(*final_set));
  }
  }

  return std::unexpected("Unknown automaton type.");
}

std::vector<std::string> AutomataDraft::transition_labels() const {
  std::vector<std::string> labels;
  labels.reserve(m_transitions.size());
  for (const auto &t : m_transitions) {
    std::string label = t.from + " -" + t.symbol + "-> ";
    for (size_t i = 0; i < t.to.size(); ++i) {
      label += (i ? "," : "") + t.to[i];
    }

    labels.push_back(label);
  }

  return labels;
}
