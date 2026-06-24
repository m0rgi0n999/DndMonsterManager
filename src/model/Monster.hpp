// Monster.hpp
#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct Monster {
    std::string name;
    int strength, dexterity, constitution, intelligence, wisdom, charisma;
    int hp, ac;
    double cr;
    std::vector<std::string> abilities;

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
