#include "screens/automata_form.h"
#include "ui/widgets.h"

#include <algorithm>
#include <ftxui/dom/elements.hpp>
#include <utility>

using namespace ftxui;

namespace {

std::string AlphabetLabel(const Alphabet &alphabet) {
  std::string label = alphabet.name() + "  {";
  bool first = true;
  for (const auto &symbol : alphabet.symbols()) {
    if (!first) {
      label += ", ";
    }

    label += symbol;
    first = false;
  }

  return label + "}";
}

} // namespace

AutomataForm::AutomataForm(
    const SystemManager &system,
    std::function<void(std::shared_ptr<AutomataBase>)> on_saved)
    : m_system(system), m_on_saved(std::move(on_saved)) {
  auto name_input =
      Input(&m_draft.name, ui::InputStyle("Words with at least one 0"));
  auto type_toggle = Menu(&m_types, &m_draft.type_index, MenuOption::Toggle());

  auto states_input = ui::ListInput(
      &m_state_buffer, "q0  [Enter] add",
      [this] { Apply(m_draft.add_state(m_state_buffer), m_state_buffer); },
      [this] { return m_draft.remove_last_state(); });

  auto finals_input = ui::ListInput(
      &m_final_buffer, "q1  [Enter] add",
      [this] { Apply(m_draft.add_final(m_final_buffer), m_final_buffer); },
      [this] { return m_draft.remove_last_final(); });

  auto transitions_input = ui::ListInput(
      &m_transition_buffer, "q0 0 q1  [Enter] add",
      [this] {
        Apply(m_draft.add_transition(m_transition_buffer,
                                     SelectedAlphabet().get()),
              m_transition_buffer);
      },
      [this] { return m_draft.remove_last_transition(); });

  auto alphabet_list = Radiobox(&m_alphabet_labels, &m_selected_alphabet) |
                       Maybe([this] { return HasAlphabets(); });

  auto initial_input = Input(&m_draft.initial, ui::InputStyle("q0"));
  auto save_button =
      Button("Save", [this] { OnSave(); }, ButtonOption::Ascii());

  auto form = Container::Vertical({
      name_input,
      type_toggle,
      states_input,
      alphabet_list,
      initial_input,
      finals_input,
      transitions_input,
      save_button,
  });

  Add(Renderer(form, [=, this] {
    RefreshAlphabets();

    Element alphabet_view =
        HasAlphabets() ? alphabet_list->Render() | vscroll_indicator | frame |
                             size(HEIGHT, LESS_THAN, 6)
                       : text("no alphabets yet. Create one first.") | dim;

    return vbox({
        ui::Field("Name", name_input),
        ui::Field("Type", type_toggle),
        ui::Field("States", states_input),
        hbox({text("  "), ui::Chips(m_draft.states(), "no states yet")}),
        hbox({ui::FieldLabel("Alphabet", alphabet_list->Focused()),
              alphabet_view}),
        ui::Field("Initial", initial_input),
        ui::Field("Finals", finals_input),
        hbox({text("  "), ui::Chips(m_draft.finals(), "no final states yet")}),
        ui::Field("Transitions", transitions_input),
        hbox({text("  "),
              ui::Chips(m_draft.transition_labels(), "no transitions yet")}),
        text("  format: <source> <symbol> <target> [<target>...]") | dim,
        text("  eNFA: eps for ε  |  Backspace on empty field removes last") |
            dim,
        separator(),
        hbox({save_button->Render(), text("  "),
              ui::Feedback(m_message, m_is_error)}),
    });
  }));
}

void AutomataForm::Apply(const Result<void> &result, std::string &buffer) {
  if (!result) {
    return ShowError(result.error());
  }

  buffer.clear();
  m_message.clear();
}

void AutomataForm::ShowError(std::string message) {
  m_message = std::move(message);
  m_is_error = true;
}

void AutomataForm::OnSave() {
  auto automata = m_draft.build(SelectedAlphabet());
  if (!automata) {
    return ShowError(automata.error());
  }

  m_message.clear();
  ClearForm();
  m_on_saved(std::move(*automata));
}

void AutomataForm::ClearForm() {
  m_draft = {};
  m_state_buffer.clear();
  m_final_buffer.clear();
  m_transition_buffer.clear();
  m_selected_alphabet = 0;
}

void AutomataForm::RefreshAlphabets() {
  const auto &alphabets = m_system.alphabets();
  m_alphabet_labels.clear();
  m_alphabet_labels.reserve(alphabets.size());
  for (const auto &a : alphabets) {
    m_alphabet_labels.push_back(AlphabetLabel(*a));
  }

  const int count = static_cast<int>(m_alphabet_labels.size());
  m_selected_alphabet =
      count == 0 ? 0 : std::clamp(m_selected_alphabet, 0, count - 1);
}

bool AutomataForm::HasAlphabets() const {
  return !m_system.alphabets().empty();
}

std::shared_ptr<Alphabet> AutomataForm::SelectedAlphabet() const {
  const auto &alphabets = m_system.alphabets();
  if (alphabets.empty()) {
    return nullptr;
  }

  return alphabets[m_selected_alphabet];
}
