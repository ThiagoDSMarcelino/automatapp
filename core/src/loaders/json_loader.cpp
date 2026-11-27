#include "loaders/json_loader.h"
#include "automatas/automata_spec.h"
#include "core/utf8_path.h"
#include "exporters/json_exporter.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>

using nlohmann::json;

namespace {

Result<AutomataSpec> spec_from_json(const json &item) {
  const auto type = parse_automata_type(item.at("type").get<std::string>());
  if (!type) {
    return std::unexpected("unknown type '" +
                           item.at("type").get<std::string>() + "'");
  }

  AutomataSpec spec;
  spec.name = item.at("name").get<std::string>();
  spec.type = *type;
  spec.alphabet = item.at("alphabet").get<std::string>();
  spec.states = item.at("states").get<std::vector<std::string>>();
  spec.initial = item.at("initial").get<std::string>();
  spec.finals = item.at("finals").get<std::vector<std::string>>();

  for (const auto &t : item.at("transitions")) {
    spec.transitions.push_back({t.at("from").get<std::string>(),
                                t.at("symbol").get<std::string>(),
                                t.at("to").get<std::vector<std::string>>()});
  }

  return spec;
}

Result<void> load_alphabet(SystemManager &system, const json &item) {
  auto alphabet =
      Alphabet::Create(item.at("name").get<std::string>(),
                       item.at("symbols").get<std::vector<Symbol>>());
  if (!alphabet) {
    return std::unexpected(alphabet.error());
  }

  return system.add_alphabet(std::move(*alphabet));
}

Result<void> load_automata(SystemManager &system, const json &item) {
  auto spec = spec_from_json(item);
  if (!spec) {
    return std::unexpected(spec.error());
  }

  auto alphabet = system.find_alphabet(spec->alphabet);
  if (alphabet == nullptr) {
    return std::unexpected("alphabet '" + spec->alphabet + "' not found");
  }

  auto automata = build_automata(*spec, std::move(alphabet));
  if (!automata) {
    return std::unexpected(automata.error());
  }

  return system.add_automata(std::move(*automata));
}

} // namespace

Result<std::unique_ptr<SystemManager>>
JsonLoader::load(const std::string &path) const {
  std::ifstream in(utf8_path(path), std::ios::binary);
  if (!in) {
    return std::unexpected("Cannot open '" + path + "'");
  }

  try {
    const json root = json::parse(in);

    const int version = root.at("version").get<int>();
    if (version != JSON_FORMAT_VERSION) {
      return std::unexpected("Unsupported file version " +
                             std::to_string(version));
    }

    auto system = std::make_unique<SystemManager>();

    const auto &alphabets = root.at("alphabets");
    for (std::size_t i = 0; i < alphabets.size(); ++i) {
      if (auto loaded = load_alphabet(*system, alphabets[i]); !loaded) {
        return std::unexpected("Alphabet #" + std::to_string(i + 1) + ": " +
                               loaded.error());
      }
    }

    const auto &automatas = root.at("automatas");
    for (std::size_t i = 0; i < automatas.size(); ++i) {
      if (auto loaded = load_automata(*system, automatas[i]); !loaded) {
        return std::unexpected(
            "Automata '" + automatas[i].value("name", std::to_string(i + 1)) +
            "': " + loaded.error());
      }
    }

    return system;
  } catch (const json::exception &error) {
    return std::unexpected("Invalid JSON in '" + path + "': " + error.what());
  }
}
