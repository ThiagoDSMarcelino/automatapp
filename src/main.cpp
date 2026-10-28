#include "alphabet.h"
#include "automatas/dfa.h"
#include "core.h"
#include "state.h"
#include "state_set.h"
#include "testing/test_battery.h"
#include "testing/test_result.h"
#include "transitions/transition_d.h"
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

std::string join(const std::vector<std::string> &parts,
                 const std::string &sep = " ") {
  std::string result;
  std::ranges::copy(parts | std::views::join_with(sep),
                    std::back_inserter(result));
  return result;
}

void print_test_battery_result(const TestBattery &battery,
                               const TestResult &result) {
  std::cout << "Running test battery \"" << battery.name()
            << "\" with automata \"" << battery.automata().name() << "\":\n";

  for (size_t i = 0; i < result.itens().size(); i++) {
    auto item = result.itens()[i];

    std::cout << i + 1 << ": \"" << join(item.word()) << "\"\n";
    std::cout << "Result: " << std::boolalpha << item.is_valid() << "\n";
    if (i + 1 != result.itens().size()) {
      std::cout << "\n";
    }
  }
}

int main() {
  auto q1 = std::make_shared<State>("q1");

  auto binary = std::make_shared<Alphabet>("Binary", Word{"0", "1"});

  auto t = std::make_shared<TransitionD>("0", q1, q1);

  auto states =
      std::make_unique<StateSet>(std::vector<std::shared_ptr<State>>{q1});

  auto final_states =
      std::make_unique<StateSet>(std::vector<std::shared_ptr<State>>{q1});

  auto dfa = std::make_shared<DFA>("Test", std::move(states), binary,
                                   std::vector<std::shared_ptr<TransitionD>>{t},
                                   q1, std::move(final_states));

  std::vector<std::vector<std::string>> words = {
      {"0"}, {"0", "0"}, {"0", "0", "1"}};

  auto battery = std::make_unique<TestBattery>("Base test", words, dfa);

  auto result = battery->run();

  print_test_battery_result(*battery, *result);

  return 0;
}
