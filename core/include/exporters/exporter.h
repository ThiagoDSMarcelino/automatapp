#pragma once

#include "automatas/automata_base.h"
#include "core/language_types.h"
#include "core/system_manager.h"
#include <string>

class Exporter {
public:
  virtual ~Exporter() = default;

  virtual Result<void> export_automata(const AutomataBase &automata,
                                       const std::string &path) const = 0;
  virtual Result<void> export_system(const SystemManager &system,
                                     const std::string &path) const = 0;
};
