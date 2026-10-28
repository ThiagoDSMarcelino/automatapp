#pragma once

#include "core.h"
#include <string>
#include <vector>

class Alphabet {
private:
  std::string m_name;
  std::vector<Symbol> m_symbols;

public:
  Alphabet(std::string name, std::vector<Symbol> symbols);

  const std::string &name() const { return m_name; };

  const std::vector<Symbol> &symbols() const { return m_symbols; };

  bool contains(const Symbol &symbol) const;
};
