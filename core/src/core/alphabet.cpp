#include "core/alphabet.h"

#include <algorithm>
#include <unordered_set>
#include <utility>

Alphabet::Alphabet(std::string name, std::vector<Symbol> symbols)
    : m_name(std::move(name)), m_symbols(std::move(symbols)) {}

Result<std::shared_ptr<Alphabet>> Alphabet::Create(std::string name,
                                                   std::vector<Symbol> symbols) {
  if (name.empty()) {
    return std::unexpected("Alphabet name cannot be empty");
  }

  if (symbols.empty()) {
    return std::unexpected("Alphabet must have at least one symbol");
  }

  std::unordered_set<Symbol> seen;
  for (const auto &symbol : symbols) {
    if (symbol.empty()) {
      return std::unexpected("Alphabet symbols cannot be empty");
    }

    if (symbol == Epsilon) {
      return std::unexpected("Alphabet cannot contain " + Epsilon);
    }

    if (!seen.insert(symbol).second) {
      return std::unexpected("Duplicate symbol in alphabet: " + symbol);
    }
  }

  return std::shared_ptr<Alphabet>(
      new Alphabet(std::move(name), std::move(symbols)));
}

bool Alphabet::contains(const Symbol &symbol) const {
  return std::ranges::find(m_symbols, symbol) != m_symbols.end();
}
