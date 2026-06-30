// Monster.hpp
// Monster.hpp
#pragma once
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

struct Monster {
  std::string name;
  int strength, dexterity, constitution, intelligence, wisdom, charisma;
  int hp, ac;
  std::vector<std::string> dmgResistances;
  std::vector<std::string> dmgImmunes;
  std::vector<std::string> condImmunes;
  std::vector<std::string> senses;
  int passivePerception;
  std::vector<std::string> languages;
  double cr;
  std::vector<std::string> abilities;
  std::vector<std::string> actions;
  std::map<std::string, int> savingThrows;
  std::map<std::string, int> skills;
  std::vector<std::string> legendaryActions;
  std::vector<std::string> reactions;
  int currentHp = 10;
  int initiative = 0;
  std::vector<std::string> templates;
  std::string notes;

  nlohmann::json to_json() const;
  static Monster from_json(const nlohmann::json& j);

  int getStat(const std::string& statName) const {
    if (statName == "Strength") return strength;
    if (statName == "Dexterity") return dexterity;
    if (statName == "Constitution") return constitution;
    if (statName == "Intelligence") return intelligence;
    if (statName == "Wisdom") return wisdom;
    if (statName == "Charisma") return charisma;
    return 10; // Default
  };

  int getModifier(int stat) const {
    return (stat - 10) / 2;
  };
};
