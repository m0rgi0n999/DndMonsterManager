// MonsterManager.hpp
#pragma once
#include "Monster.hpp"
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>

class MonsterManager {
  const std::filesystem::path storageDir = "data/monsters";
public:
  MonsterManager() { std::filesystem::create_directories(storageDir); }

  void saveMonster(const Monster& m) {
    std::ofstream file(storageDir / (m.name + ".json"));
    file << m.to_json().dump(4);
  }

	void deleteMonster(const std::string& name) {
    std::filesystem::path filePath = storageDir / (name + ".json");
        
    // Simply delete the file from the disk
    if (std::filesystem::exists(filePath)) {
      std::filesystem::remove(filePath);
    }
  }

  std::vector<Monster> loadAll() {
    std::vector<Monster> monsters;
    if (!std::filesystem::exists(storageDir)) return monsters;

    for (auto& p : std::filesystem::directory_iterator(storageDir)) {
      std::ifstream file(p.path());
      if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        monsters.push_back(Monster::from_json(j));
      }
    }
    return monsters;
  }

	void updateMonster(const std::string& oldName, const Monster& newMonster) {
		if (oldName != newMonster.name) {
			deleteMonster(oldName); // Delete the old file
		}
		saveMonster(newMonster);    // Save the new one
	}

	Monster findMonster(const std::string& name) {
		nlohmann::json j;
		std::ifstream file(storageDir / (name + ".json"));
		if (file.is_open()) {
			file >> j;
			return Monster::from_json(j);
		}
		return {}; // Return empty Monster if not found.
	}
};
