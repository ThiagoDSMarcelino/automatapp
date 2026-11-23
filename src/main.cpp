#include "core/alphabet.h"
#include "core/system_manager.h"

int main() {
  SystemManager systemManager;

  auto binary = Alphabet::Create("Binary", {"0", "1"});
  auto boolean = Alphabet::Create("Boolean", {"true", "false"});

  systemManager.add_alphabet(binary.value());
  systemManager.add_alphabet(boolean.value());

  return 0;
}
