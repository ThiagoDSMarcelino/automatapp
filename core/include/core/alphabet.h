#pragma once

#include "core/language_types.h"
#include <memory>
#include <string>
#include <vector>

class Alphabet {
public:
  static Result<std::shared_ptr<Alphabet>> Create(std::string name,
                                                  std::vector<Symbol> symbols);
  const std::string &name() const { return m_name; };
  const std::vector<Symbol> &symbols() const { return m_symbols; };
  bool contains(const Symbol &symbol) const;

private:
  std::string m_name;
  std::vector<Symbol> m_symbols;

  Alphabet(std::string name, std::vector<Symbol> symbols);
};
