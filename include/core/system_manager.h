#pragma once

#include "automatas/automata_base.h"
#include "core/alphabet.h"
#include <memory>
#include <utility>
#include <vector>

class SystemManager {
public:
  const std::vector<std::shared_ptr<Alphabet>> &alphabets() const {
    return m_alphabets;
  };

  void add_alphabet(std::shared_ptr<Alphabet> alphabet) {
    m_alphabets.push_back(std::move(alphabet));
  }

  const std::vector<std::shared_ptr<AutomataBase>> &automatas() const {
    return m_automatas;
  };

  void add_automata(std::shared_ptr<AutomataBase> automata) {
    m_automatas.push_back(std::move(automata));
  }

private:
  std::vector<std::shared_ptr<Alphabet>> m_alphabets;
  std::vector<std::shared_ptr<AutomataBase>> m_automatas;
};
