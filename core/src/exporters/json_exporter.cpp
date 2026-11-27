#include "exporters/json_exporter.h"
#include "automatas/automata_spec.h"
#include "core/utf8_path.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>

using json = nlohmann::ordered_json;

namespace {

json alphabet_to_json(const Alphabet &alphabet) {
  return {{"name", alphabet.name()}, {"symbols", alphabet.symbols()}};
}

json automata_to_json(const AutomataBase &automata) {
  const AutomataSpec spec = to_spec(automata);

  json transitions = json::array();
  for (const auto &t : spec.transitions) {
    transitions.push_back(
        {{"from", t.from}, {"symbol", t.symbol}, {"to", t.to}});
  }

  return {{"name", spec.name},         {"type", automata_type_name(spec.type)},
          {"alphabet", spec.alphabet}, {"states", spec.states},
          {"initial", spec.initial},   {"finals", spec.finals},
          {"transitions", transitions}};
}

json document(json alphabets, json automatas) {
  return {{"version", JSON_FORMAT_VERSION},
          {"alphabets", std::move(alphabets)},
          {"automatas", std::move(automatas)}};
}

Result<void> write_atomically(const json &content, const std::string &path) {
  const std::filesystem::path target = utf8_path(path);
  std::filesystem::path temporary = target;
  temporary += ".tmp";

  {
    std::ofstream out(temporary, std::ios::binary | std::ios::trunc);
    if (!out) {
      return std::unexpected("Cannot open '" + path + ".tmp' for writing");
    }

    out << content.dump(2) << '\n';
    if (!out.flush()) {
      return std::unexpected("Cannot write '" + path + ".tmp'");
    }
  }

  std::error_code error;
  std::filesystem::rename(temporary, target, error);
  if (error) {
    const std::string reason = error.message();
    std::filesystem::remove(temporary, error);
    return std::unexpected("Cannot replace '" + path + "': " + reason);
  }

  return {};
}

} // namespace

Result<void> JsonExporter::export_automata(const AutomataBase &automata,
                                           const std::string &path) const {
  return write_atomically(
      document(json::array({alphabet_to_json(*automata.alphabet())}),
               json::array({automata_to_json(automata)})),
      path);
}

Result<void> JsonExporter::export_system(const SystemManager &system,
                                         const std::string &path) const {
  json alphabets = json::array();
  for (const auto &alphabet : system.alphabets()) {
    alphabets.push_back(alphabet_to_json(*alphabet));
  }

  json automatas = json::array();
  for (const auto &automata : system.automatas()) {
    automatas.push_back(automata_to_json(*automata));
  }

  return write_atomically(document(std::move(alphabets), std::move(automatas)),
                          path);
}
