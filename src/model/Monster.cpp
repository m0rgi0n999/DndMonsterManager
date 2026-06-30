// Monster.cpp
#include "Monster.hpp"

nlohmann::json Monster::to_json() const {
  return nlohmann::json{
    {"name", name},
	{"str", strength},
	{"dex", dexterity},
	{"con", constitution},
    {"int", intelligence},
	{"wis", wisdom},
	{"cha", charisma},
    {"hp", hp},
	{"ac", ac},
	{"dmgResistances", dmgResistances},
	{"dmgImmunes", dmgImmunes},
	{"condImmunes", condImmunes},
	{"senses", senses},
    {"passivePerception", passivePerception},
	{"languages", languages},
	{"cr", cr},
	{"abilities", abilities},
	{"actions", actions},
	{"savingThrows", savingThrows},
	{"skills", skills},
	{"legendaryActions", legendaryActions},
	{"reactions", reactions},
	{"currentHp", currentHp},
	{"initiative", initiative},
	{"templates", templates},
	{"notes", notes}
  };
}

Monster Monster::from_json(const nlohmann::json& j) {
  return {
    j.value("name", "Unknown"),
    j.value("str", 10),
    j.value("dex", 10),
    j.value("con", 10),
    j.value("int", 10),
    j.value("wis", 10),
    j.value("cha", 10),
    j.value("hp", 10),
    j.value("ac", 10),
	j.value("dmgResistances", std::vector<std::string>{}),
	j.value("dmgImmunes", std::vector<std::string>{}),
	j.value("condImmunes", std::vector<std::string>{}),
	j.value("senses", std::vector<std::string>{}),
    j.value("passivePerception", 10), // Safe: if missing, defaults to 10
	j.value("languages", std::vector<std::string>{}),
    j.value("cr", 0.0),               // Safe: if missing, defaults to 0.0
    j.value("abilities", std::vector<std::string>{}), // Safe: if missing, defaults to empty list
	j.value("actions", std::vector<std::string>{}),
	j.value("savingThrows", std::map<std::string, int>{}),
	j.value("skills", std::map<std::string, int>{}),
	j.value("legendaryActions", std::vector<std::string>{}),
	j.value("reactions", std::vector<std::string>{}),
	j.value("currentHp", j.value("hp", 10)),
	j.value("initiative", 0),
	j.value("templates", std::vector<std::string>{}),
	j.value("notes", "")
  };
}
