#pragma once
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include "Monster.hpp"

struct TemplateDefinition {
  std::string name;
  std::string description;
  std::function<void(Monster&)> apply;
};

inline std::vector<TemplateDefinition> getTemplateDefinitions() {
  return {
    {"Undead", "Adds undead resilience and bonus HP", [](Monster& m) {
      if (std::find(m.abilities.begin(), m.abilities.end(), "Undead Fortitude") == m.abilities.end()) {
        m.abilities.push_back("Undead Fortitude");
      }
      m.hp += 10;
      m.currentHp = std::max(m.currentHp, m.hp);
    }},
    {"Brute", "Boosts durability and strength", [](Monster& m) {
      m.hp += 15;
      m.currentHp = std::max(m.currentHp, m.hp);
      m.strength += 2;
      if (std::find(m.abilities.begin(), m.abilities.end(), "Brute") == m.abilities.end()) {
        m.abilities.push_back("Brute");
      }
    }},
    {"Spellcaster", "Adds initiative and spellcasting flavor", [](Monster& m) {
      m.initiative += 2;
      if (std::find(m.actions.begin(), m.actions.end(), "Spellcasting") == m.actions.end()) {
        m.actions.push_back("Spellcasting");
      }
    }},
    {"Skirmisher", "Improves mobility and initiative", [](Monster& m) {
      m.initiative += 3;
      m.dexterity += 2;
      if (std::find(m.abilities.begin(), m.abilities.end(), "Skirmisher") == m.abilities.end()) {
        m.abilities.push_back("Skirmisher");
      }
    }}
  };
}

inline void applyTemplate(Monster& m, const std::string& name) {
  for (const auto& definition : getTemplateDefinitions()) {
    if (definition.name == name) {
      definition.apply(m);
      if (std::find(m.templates.begin(), m.templates.end(), name) == m.templates.end()) {
        m.templates.push_back(name);
      }
      break;
    }
  }
}
