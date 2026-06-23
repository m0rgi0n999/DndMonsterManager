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
		{"cr", cr},
		{"abilities", abilities}
    };
}

Monster Monster::from_json(const nlohmann::json& j) {
    return {
        j.at("name").get<std::string>(),
        j.at("str"),
		j.at("dex"),
		j.at("con"),
		j.at("int"),
		j.at("wis"),
		j.at("cha"),
        j.at("hp"),
		j.at("ac"),
		j.at("cr"),
        j.at("abilities").get<std::vector<std::string>>()
    };
}