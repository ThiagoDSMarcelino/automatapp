#include "core/system_manager.h"

#include <algorithm>
#include <utility>

namespace {

auto by_name(const std::string &name) {
  return [&name](const std::shared_ptr<Alphabet> &a) {
    return a->name() == name;
  };
}

} // namespace

std::shared_ptr<Alphabet>
SystemManager::find_alphabet(const std::string &name) const {
  auto it = std::ranges::find_if(m_alphabets, by_name(name));
  return it == m_alphabets.end() ? nullptr : *it;
}

bool SystemManager::is_alphabet_in_use(const std::string &name) const {
  return std::ranges::any_of(
      m_automatas, [&](const std::shared_ptr<AutomataBase> &automata) {
        return automata->alphabet()->name() == name;
      });
}

Result<void> SystemManager::add_alphabet(std::shared_ptr<Alphabet> alphabet) {
  if (alphabet == nullptr) {
    return std::unexpected("Alphabet cannot be NULL");
  }

  if (find_alphabet(alphabet->name()) != nullptr) {
    return std::unexpected("Alphabet '" + alphabet->name() +
                           "' already exists");
  }

  m_alphabets.push_back(std::move(alphabet));
  return {};
}

Result<void>
SystemManager::replace_alphabet(const std::string &name,
                                std::shared_ptr<Alphabet> alphabet) {
  if (alphabet == nullptr) {
    return std::unexpected("Alphabet cannot be NULL");
  }

  auto it = std::ranges::find_if(m_alphabets, by_name(name));
  if (it == m_alphabets.end()) {
    return std::unexpected("Alphabet '" + name + "' does not exist");
  }

  if (alphabet->name() != name && find_alphabet(alphabet->name()) != nullptr) {
    return std::unexpected("Alphabet '" + alphabet->name() +
                           "' already exists");
  }

  if (is_alphabet_in_use(name)) {
    return std::unexpected("Alphabet '" + name +
                           "' is used by an automata and cannot be changed");
  }

  *it = std::move(alphabet);
  return {};
}

Result<void> SystemManager::remove_alphabet(const std::string &name) {
  auto it = std::ranges::find_if(m_alphabets, by_name(name));
  if (it == m_alphabets.end()) {
    return std::unexpected("Alphabet '" + name + "' does not exist");
  }

  if (is_alphabet_in_use(name)) {
    return std::unexpected("Alphabet '" + name +
                           "' is used by an automata and cannot be removed");
  }

  m_alphabets.erase(it);
  return {};
}
