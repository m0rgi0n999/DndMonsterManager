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
  {"passivePerception", passivePerception},
	{"cr", cr},
	{"abilities", abilities}
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
    j.value("passivePerception", 10), // Safe: if missing, defaults to 10
    j.value("cr", 0.0),               // Safe: if missing, defaults to 0.0
    j.value("abilities", std::vector<std::string>{}) // Safe: if missing, defaults to empty list
  };
}
