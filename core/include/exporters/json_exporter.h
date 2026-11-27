#pragma once

#include "exporters/exporter.h"

inline constexpr int JSON_FORMAT_VERSION = 1;

class JsonExporter : public Exporter {
public:
  Result<void> export_automata(const AutomataBase &automata,
                               const std::string &path) const override;
  Result<void> export_system(const SystemManager &system,
                             const std::string &path) const override;
};
