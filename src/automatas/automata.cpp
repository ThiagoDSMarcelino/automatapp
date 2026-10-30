#include "automatas/automata.h"
#include "state_set.h"

#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

Automata::Automata(std::string name, std::unique_ptr<StateSet> states,
                   std::shared_ptr<Alphabet> alphabet,
                   std::vector<std::shared_ptr<Transition>> transitions,
                   std::shared_ptr<State> initial_state,
                   std::unique_ptr<StateSet> final_states)
    : m_name(std::move(name)), m_states(std::move(states)),
      m_alphabet(std::move(alphabet)), m_transitions(std::move(transitions)),
      m_initial_state(std::move(initial_state)),
      m_final_states(std::move(final_states)) {
  if (this->m_name.empty()) {
    throw std::invalid_argument("Automata name cannot be empty");
  }

  if (this->m_states == nullptr) {
    throw std::invalid_argument("Automata states cannot be NULL");
  }

  if (this->m_alphabet == nullptr) {
    throw std::invalid_argument("Automata alphabet cannot be NULL");
  }

  if (this->m_initial_state == nullptr) {
    throw std::invalid_argument("Automata initial state cannot be NULL");
  }

  if (!this->m_states->contains(this->m_initial_state)) {
    throw std::invalid_argument(
        "Automata initial state does not belong to the Automata");
  }

  if (this->m_final_states == nullptr) {
    throw std::invalid_argument("Automata final states cannot be NULL");
  }

  for (const auto &state : *this->m_final_states) {
    if (!this->m_states->contains(state)) {
      throw std::invalid_argument(
          "One of the Automata final states does not belong to the Automata");
    }
  }

  for (const auto &state : *this->m_states) {
    m_transitions_by_state[state];
  }

  std::set<std::pair<std::shared_ptr<State>, std::string>> seen;
  for (const auto &transition : this->m_transitions) {
    if (transition == nullptr) {
      throw std::invalid_argument("Automata transition cannot be NULL");
    }

    if (!transition->from()->name().empty()) {
      auto pair = std::make_pair(transition->from(), transition->symbol());
      if (!seen.insert(pair).second) {
        throw std::invalid_argument("Duplicate transition from state " +
                                    transition->from()->name() +
                                    " for symbol " + transition->symbol());
      }
    }

    if (!this->m_states->contains(transition->from())) {
      throw std::invalid_argument(
          "Transition origin state does not belong to the Automata");
    }

    for (const auto &destination : transition->to()) {
      if (!this->m_states->contains(destination)) {
        throw std::invalid_argument(
            "Transition destination state does not belong to the Automata");
      }
    }

    m_transitions_by_state[transition->from()].push_back(transition);
  }
}

bool Automata::accept(const std::vector<std::string> &word) const {
  std::vector<std::shared_ptr<State>> current_states = {this->m_initial_state};
  std::vector<std::shared_ptr<State>> next_states;

  for (auto &symbol : word) {
    if (!this->m_alphabet->contains(symbol)) {
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
