#include "automatas/automata_base.h"
#include "core/language_types.h"
#include "core/state_set.h"

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

AutomataBase::AutomataBase(std::string name, std::unique_ptr<StateSet> states,
                           std::shared_ptr<Alphabet> alphabet,
                           std::vector<std::shared_ptr<Transition>> transitions,
                           std::shared_ptr<State> initial_state,
                           std::unique_ptr<StateSet> final_states)
    : m_name(std::move(name)), m_states(std::move(states)),
      m_alphabet(std::move(alphabet)), m_transitions(std::move(transitions)),
      m_initial_state(std::move(initial_state)),
      m_final_states(std::move(final_states)) {
  for (const auto &state : *this->m_states) {
    m_transitions_by_state[state];
  }

  for (const auto &transition : this->m_transitions) {
    m_transitions_by_state[transition->from()].push_back(transition);
  }
}

Result<void> AutomataBase::Validate(
    const std::string &name, const std::unique_ptr<StateSet> &states,
    const std::shared_ptr<Alphabet> &alphabet,
    const std::vector<std::shared_ptr<Transition>> &transitions,
    const std::shared_ptr<State> &initial_state,
    const std::unique_ptr<StateSet> &final_states) {
  if (name.empty()) {
    return std::unexpected("Automata name cannot be empty");
  }

  if (states == nullptr) {
    return std::unexpected("Automata states cannot be NULL");
  }

  if (alphabet == nullptr) {
    return std::unexpected("Automata alphabet cannot be NULL");
  }

  if (initial_state == nullptr) {
    return std::unexpected("Automata initial state cannot be NULL");
  }

  if (!states->contains(initial_state)) {
    return std::unexpected(
        "Automata initial state does not belong to the Automata");
  }

  if (final_states == nullptr) {
    return std::unexpected("Automata final states cannot be NULL");
  }

  for (const auto &state : *final_states) {
    if (!states->contains(state)) {
      return std::unexpected(
          "One of the Automata final states does not belong to the Automata");
    }
  }

  std::set<std::pair<std::shared_ptr<State>, std::string>> seen;
  for (const auto &transition : transitions) {
    if (transition == nullptr) {
      return std::unexpected("Automata transition cannot be NULL");
    }

    const Symbol &symbol = transition->symbol();
    if (symbol != Epsilon && !alphabet->contains(symbol)) {
      return std::unexpected("Symbol '" + symbol + "' is not in alphabet '" +
                             alphabet->name() + "'");
    }

    auto pair = std::make_pair(transition->from(), transition->symbol());
    if (!seen.insert(pair).second) {
      return std::unexpected("Duplicate transition from state " +
                             *transition->from() + " for symbol " +
                             transition->symbol());
    }

    if (!states->contains(transition->from())) {
      return std::unexpected(
          "Transition origin state does not belong to the Automata");
    }

    for (const auto &destination : transition->to()) {
      if (!states->contains(destination)) {
        return std::unexpected(
            "Transition destination state does not belong to the Automata");
      }
    }
  }

  return {};
}

bool AutomataBase::accept(const std::vector<std::string> &word) const {
  std::vector<std::shared_ptr<State>> current_states = {this->m_initial_state};
  std::vector<std::shared_ptr<State>> next_states;

  for (auto &symbol : word) {
    if (symbol != Epsilon && !this->m_alphabet->contains(symbol)) {
      return false;
    }

    for (auto state : current_states) {
      const auto &transitions = this->m_transitions_by_state.at(state);

      for (auto t : transitions) {
        if (t->symbol() != symbol) {
          continue;
        }

        for (auto next : t->to()) {
          next_states.push_back(next);
        }
      }
    }

    if (next_states.empty()) {
      return false;
    }

    std::swap(current_states, next_states);
    next_states.clear();
  }

  for (const auto &state : current_states) {
    if (this->m_final_states->contains(state)) {
      return true;
    }
  }

  return false;
}
