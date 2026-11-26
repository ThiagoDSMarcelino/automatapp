#pragma once

#include "automatas/automata_base.h"
#include "core/alphabet.h"
#include "core/language_types.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

class SystemManager {
public:
  const std::vector<std::shared_ptr<Alphabet>> &alphabets() const {
    return m_alphabets;
  };

  std::shared_ptr<Alphabet> find_alphabet(const std::string &name) const;
  bool is_alphabet_in_use(const std::string &name) const;

  Result<void> add_alphabet(std::shared_ptr<Alphabet> alphabet);
  Result<void> replace_alphabet(const std::string &name,
                                std::shared_ptr<Alphabet> alphabet);
  Result<void> remove_alphabet(const std::string &name);

  const std::vector<std::shared_ptr<AutomataBase>> &automatas() const {
    return m_automatas;
  };

  std::shared_ptr<AutomataBase> find_automata(const std::string &name) const;

  Result<void> add_automata(std::shared_ptr<AutomataBase> automata);
  Result<void> replace_automata(const std::string &name,
                                std::shared_ptr<AutomataBase> automata);
  Result<void> remove_automata(const std::string &name);

private:
  std::vector<std::shared_ptr<Alphabet>> m_alphabets;
  std::vector<std::shared_ptr<AutomataBase>> m_automatas;
};
