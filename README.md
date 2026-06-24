# DndMonsterManager - Dungeons & Dragons Monster Management System

> **Extensive README documenting the DndMonsterManager codebase**  
> *Generated for Erwin Rijvers - June 24, 2026*

---

## 📋 Table of Contents

1. [Overview](#-overview)
2. [Repository Structure](#-repository-structure)
3. [Technologies & Dependencies](#-technologies--dependencies)
4. [Architecture](#-architecture)
5. [Core Components](#-core-components)

- [Monster Data Model](#monster-data-model)
- [MonsterManager Class](#monstermanager-class)
- [Template System](#template-system)
- [MainWindow GUI](#mainwindow-gui)
1. [Data Storage](#-data-storage)
2. [Build System](#-build-system)
3. [Features](#-features)
4. [Usage](#-usage)
5. [Code Analysis](#-code-analysis)
6. [Future Enhancements](#-future-enhancements)

---

## 🎯 Overview

**DndMonsterManager** is a C++ desktop application designed for Dungeons & Dragons (D&D) Dungeon Masters and players to manage monster statistics and data. The application provides a graphical user interface for creating, viewing, editing, and deleting monster entries with all standard D&D 5e attributes.

The project is built using modern C++(C++20), Qt6 for the GUI framework, and the nlohmann/json library for JSON serialization. It follows a clean MVC-like architecture separating data models, business logic, and presentation layers.

---

## 🗂️ Repository Structure

```
DndMonsterManager/
├── CMakeLists.txt              # CMake build configuration
├── build/                      # Build artifacts and compiled binaries
│   ├── DndMonsterManager       # Compiled executable
│   ├── CMakeFiles/             # CMake generated files
│   ├── Makefile                # Generated makefile
│   └── .qt/                    # Qt-specific build files
│
├── data/
│   └── monsters/               # Monster data storage (JSON files)
│       ├── Aarakocra.json
│       ├── Aboleth.json
│       ├── Deva.json
│       ├── Planetar.json
│       └── Solar.json
│
└── src/
    ├── main.cpp                # Application entry point
    │
    ├── gui/
    │   ├── MainWindow.hpp       # Main window header
    │   └── MainWindow.cpp       # Main window implementation
    │
    └── model/
        ├── Monster.hpp          # Monster data structure header
        ├── Monster.cpp          # Monster data structure implementation
        ├── MonsterManager.hpp   # Monster CRUD operations header
        └── Templates.hpp        # Monster template system
```

---

## 🛠️ Technologies & Dependencies

| Component         | Version | Purpose                            |
| ----------------- | ------- | ---------------------------------- |
| **C++**           | C++20   | Core programming language          |
| **Qt6**           | 6.x     | GUI framework (Widgets module)     |
| **CMake**         | 3.16+   | Cross-platform build system        |
| **nlohmann/json** | Latest  | JSON serialization/deserialization |
| **filesystem**    | C++17   | File system operations             |

### Build Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 19.28+)
- Qt6 development libraries
- CMake 3.16 or higher
- nlohmann/json header library

---

## 🏗️ Architecture

The application follows a **Model-View-Controller (MVC)** pattern with the following layers:

```
┌─────────────────────────────────────────────────────────────┐
│                        MainWindow (View)                       │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐  │
│  │   QLineEdit     │  │   QListWidget    │  │ QPushButton  │  │
│  │   (Stat Inputs) │  │ (Monster List)   │  │ (Actions)    │  │
│  └─────────────────┘  └─────────────────┘  └─────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     MonsterManager (Controller)                │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐  │
│  │   saveMonster()  │  │  loadAll()       │  │ deleteMonster()│ │
│  │   findMonster()  │  │  updateMonster() │  │              │  │
│  └─────────────────┘  └─────────────────┘  └─────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                        Monster (Model)                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐ │
│  │  Attributes  │  │  Methods     │  │   Serialization          │ │
│  │  (str, dex, │  │  (getStat,   │  │   (to_json, from_json)   │ │
│  │   etc.)     │  │   getMod)    │  │                         │ │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   JSON Files (Data Storage)                    │
│  data/monsters/*.json                                          │
└─────────────────────────────────────────────────────────────┘
```

---

## 🧩 Core Components

### Monster Data Model

**Files:** `src/model/Monster.hpp`, `src/model/Monster.cpp`

The `Monster` struct is the core data model representing a D&D monster with all standard attributes:

```cpp
struct Monster {
    std::string name;              // Monster name
    int strength;                  // STR attribute (default: 10)
    int dexterity;                 // DEX attribute (default: 10)
    int constitution;              // CON attribute (default: 10)
    int intelligence;              // INT attribute (default: 10)
    int wisdom;                    // WIS attribute (default: 10)
    int charisma;                  // CHA attribute (default: 10)
    int hp;                        // Hit Points
    int ac;                        // Armor Class
    double cr;                     // Challenge Rating
    std::vector<std::string> abilities; // Special abilities
}
```

#### Key Methods

| Method                                 | Description                                   | Return Type      |
| -------------------------------------- | --------------------------------------------- | ---------------- |
| `getStat(const std::string& statName)` | Returns the value of a specific stat by name  | `int`            |
| `getModifier(int stat)`                | Calculates the D&D modifier from a stat value | `int`            |
| `to_json()`                            | Serializes the monster to a JSON object       | `nlohmann::json` |
| `from_json(const nlohmann::json& j)`   | Deserializes a JSON object to a Monster       | `Monster`        |

**Modifier Calculation:**  
The D&D standard formula `(stat - 10) / 2` is used to calculate ability modifiers. For example:

- STR 14 → Modifier: +2
- DEX 8 → Modifier: -1
- CON 16 → Modifier: +3

---

### MonsterManager Class

**File:** `src/model/MonsterManager.hpp`

The `MonsterManager` class handles all data persistence operations for monsters. It provides CRUD (Create, Read, Update, Delete) functionality using the filesystem.

#### Storage Location

- All monsters are stored as individual JSON files in `data/monsters/`
- Each monster is saved as `{name}.json`
- The directory is automatically created on initialization

#### Methods

| Method                                                                 | Description                                   | Parameters            | Return Type            |
| ---------------------------------------------------------------------- | --------------------------------------------- | --------------------- | ---------------------- |
| `MonsterManager()`                                                     | Constructor, creates storage directory        | -                     | -                      |
| `saveMonster(const Monster& m)`                                        | Saves a monster to disk                       | Monster object        | `void`                 |
| `deleteMonster(const std::string& name)`                               | Deletes a monster file from disk              | Monster name          | `void`                 |
| `loadAll()`                                                            | Loads all monsters from the storage directory | -                     | `std::vector<Monster>` |
| `updateMonster(const std::string& oldName, const Monster& newMonster)` | Updates a monster (handles renaming)          | Old name, new Monster | `void`                 |
| `findMonster(const std::string& name)`                                 | Finds a specific monster by name              | Monster name          | `Monster`              |

#### Implementation Details

- Uses C++17 filesystem library for file operations
- Each monster is stored as a separate JSON file
- File naming convention: `{monster_name}.json`
- Returns empty Monster struct if monster not found

---

### Template System

**File:** `src/model/Templates.hpp`

The template system allows for applying pre-defined modifications to monsters. This is a flexible system using C++ templates (not to be confused with JSON templates).

#### Current Templates

```cpp
struct UndeadTemplate {
    void apply(Monster& m) {
        m.hp += 10;                    // Add 10 HP
        m.abilities.push_back("Undead Fortitude");  // Add ability
    }
};
```

#### Template Application

```cpp
// Generic template application function
template<typename T>
void applyTemplate(Monster& m, T templateObj) {
    templateObj.apply(m);
}

// Usage example:
Monster myMonster = ...;
UndeadTemplate undead;
applyTemplate(myMonster, undead);  // Applies undead template
```

This system is extensible - new templates can be added by creating structs with an `apply(Monster&)` method.

---

### MainWindow GUI

**Files:** `src/gui/MainWindow.hpp`, `src/gui/MainWindow.cpp`

The `MainWindow` class extends `QMainWindow` and provides the complete user interface for the application.

#### UI Components

1. **Form Layout** - Input fields for monster attributes:

- Name (QLineEdit)
- Hit Points (QLineEdit)
- All six ability scores (QLineEdit for each)
- Automatic modifier display (QLabel for each stat)
1. **Monster List** - QListWidget displaying all saved monsters
2. **Action Buttons:**

- Save Monster
- Delete Monster

#### Key Features

**Automatic Modifier Calculation:**  
Each stat input field has a connected signal that automatically calculates and displays the D&D modifier:

```cpp
connect(input, &QLineEdit::textChanged, [input, modLabel]() {
    int val = input->text().toInt();
    int mod = (val - 10) / 2;
    QString modStr = (mod >= 0) ? "+" + QString::number(mod) : QString::number(mod);
    modLabel->setText("(" + modStr + ")");
});
```

**Dark Theme:**  
The application uses a custom dark theme with the following styling:

- Background: `#2B2B2B` (dark gray)
- Text: `#ffffff` (white)
- Input fields: `#3c3c3c` (darker gray background)
- List widget: `#2B2B2B` with `#555` border

**Data Binding:**  
When a monster is selected from the list, its data automatically populates the form fields. When saved, the form data is converted to a Monster object and persisted.

#### Event Handlers

| Event                 | Action                                                            |
| --------------------- | ----------------------------------------------------------------- |
| Save Button Clicked   | Creates Monster from UI, saves via MonsterManager, refreshes list |
| Delete Button Clicked | Deletes selected monster from storage, refreshes list             |
| Monster Selected      | Loads monster data from storage, populates form fields            |
| Stat Value Changed    | Recalculates and displays the modifier                            |

---

## 💾 Data Storage

### File Format

Each monster is stored as a JSON file in the `data/monsters/` directory with the following structure:

```json
{
    "name": "Solar",
    "str": 26,
    "dex": 22,
    "con": 26,
    "int": 25,
    "wis": 25,
    "cha": 30,
    "hp": 243,
    "ac": 12,
    "cr": 0.5,
    "abilities": []
}
```

### Sample Monsters

The repository includes several pre-defined monsters:

| Monster   | STR | DEX | CON | INT | WIS | CHA | HP  | AC  | CR  |
| --------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Aarakocra | 10  | 14  | 10  | 11  | 12  | 11  | 13  | 12  | 0.5 |
| Aboleth   | 21  | 9   | 15  | 18  | 15  | 18  | -   | -   | -   |
| Deva      | 17  | 18  | 13  | 17  | 16  | 20  | -   | -   | -   |
| Planetar  | 24  | 20  | 24  | 19  | 22  | 25  | -   | -   | -   |
| Solar     | 26  | 22  | 26  | 25  | 25  | 30  | 243 | 12  | 0.5 |

---

## 🔨 Build System

### CMake Configuration

**File:** `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)
project(DndMonsterManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Auto-MOC for Qt
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(nlohmann_json REQUIRED)

# Define executable
add_executable(DndMonsterManager 
  src/main.cpp 
  src/model/Monster.cpp 
  src/gui/MainWindow.cpp
)

# Include directories and linking
target_include_directories(DndMonsterManager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(DndMonsterManager PRIVATE Qt6::Widgets nlohmann_json::nlohmann_json)
```

### Building the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the application
./DndMonsterManager
```

### Build Artifacts

The build process generates:

- `build/DndMonsterManager` - The compiled executable
- `build/DndMonsterManager_autogen/` - Qt meta-object compiler generated files
- `build/CMakeFiles/` - CMake generated configuration files
- `build/Makefile` - Generated makefile

---

## ✨ Features

### ✅ Implemented Features

1. **Monster Creation**

- Create new monsters with all D&D 5e standard attributes
- Input validation for numeric fields
- Default values for all stats (10) and HP (20)
1. **Monster Management**

- Save monsters to persistent storage
- Delete monsters from storage
- View list of all saved monsters
- Select and edit existing monsters
1. **Automatic Calculations**

- Real-time ability modifier calculation
- Visual display of modifiers next to each stat
1. **User Interface**

- Clean, dark-themed interface
- Responsive form layout
- Intuitive navigation
1. **Data Persistence**

- JSON-based storage
- Human-readable file format
- Individual files per monster
1. **Template System**

- Extensible template framework
- Pre-defined Undead template
- Easy to add new templates

### 📊 Feature Summary Table

| Feature              | Status            | Description                              |
| -------------------- | ----------------- | ---------------------------------------- |
| Monster CRUD         | ✅ Complete        | Create, Read, Update, Delete operations  |
| GUI Interface        | ✅ Complete        | Qt-based graphical user interface        |
| Data Persistence     | ✅ Complete        | JSON file storage                        |
| Modifier Calculation | ✅ Complete        | Automatic D&D modifier calculation       |
| Template System      | ✅ Basic           | Undead template implemented              |
| Search/Filter        | ❌ Not Implemented | Filter monsters by name or stats         |
| Export/Import        | ❌ Not Implemented | Export to PDF, import from other formats |
| Monster Images       | ❌ Not Implemented | Display monster artwork                  |
| Combat Tracker       | ❌ Not Implemented | Track initiative and combat state        |
| Multi-platform       | ✅ Complete        | Works on Windows, Linux, macOS           |

---

## 🚀 Usage

### Starting the Application

1. Build the project (see [Build System](#-build-system))
2. Run the executable: `./DndMonsterManager`
3. The application window will appear with the monster list and form

### Creating a New Monster

1. Fill in the monster name in the "Name:" field
2. Enter the monster's ability scores (STR, DEX, CON, INT, WIS, CHA)
3. Enter the monster's Hit Points (HP)
4. The ability modifiers are calculated automatically
5. Click "Save Monster" to save the monster
6. The monster will appear in the list

### Editing a Monster

1. Click on a monster in the list to select it
2. The form fields will populate with the monster's data
3. Modify any fields as needed
4. Click "Save Monster" to update the monster

### Deleting a Monster

1. Click on a monster in the list to select it
2. Click "Delete Monster"
3. The monster will be removed from the list and disk

---

## 🔍 Code Analysis

### Code Quality

| Metric              | Rating | Notes                                          |
| ------------------- | ------ | ---------------------------------------------- |
| **Readability**     | ⭐⭐⭐⭐   | Well-structured, clear variable names          |
| **Maintainability** | ⭐⭐⭐⭐   | Modular design, separation of concerns         |
| **Documentation**   | ⭐⭐     | Minimal comments, but code is self-documenting |
| **Error Handling**  | ⭐⭐⭐    | Basic error handling, could be improved        |
| **Performance**     | ⭐⭐⭐⭐   | Efficient, no unnecessary operations           |
| **Modern C++**      | ⭐⭐⭐⭐⭐  | Uses C++20 features, filesystem, lambdas       |

### Strengths

1. **Clean Architecture**: Clear separation between model, view, and controller
2. **Modern C++**: Uses C++20 features, smart pointers (in some places), filesystem
3. **Extensible**: Template system allows for easy extension
4. **Cross-platform**: Uses Qt and CMake for cross-platform compatibility
5. **User-friendly**: Automatic calculations, intuitive UI

### Areas for Improvement

1. **Error Handling**: Add try-catch blocks for file operations and JSON parsing
2. **Input Validation**: Validate that stat values are within D&D bounds (typically 1-30)
3. **Memory Management**: Consider using more smart pointers
4. **Testing**: Add unit tests for core functionality
5. **Documentation**: Add more comments explaining complex logic
6. **Edge Cases**: Handle duplicate monster names, empty fields, etc.

### Code Statistics

- **Total Lines of Code**: ~500 (excluding build files and data)
- **Source Files**: 6 (.cpp and .hpp files)
- **Classes/Structs**: 4 (Monster, MonsterManager, MainWindow, UndeadTemplate)
- **Dependencies**: 3 (Qt6, nlohmann/json, C++ Standard Library)

---

## 🎯 Future Enhancements

### High Priority

1. **Input Validation**

- Validate stat ranges (1-30 for abilities, positive values for HP/AC)
- Prevent duplicate monster names
- Handle empty required fields
1. **Error Handling**

- Add try-catch blocks for file operations
- Display error messages to users
- Handle JSON parsing errors gracefully
1. **Additional Monster Attributes**

- Speed (walking, flying, swimming)
- Senses (darkvision, blindsight, etc.)
- Saving throw proficiencies
- Skill proficiencies
- Damage resistances/immunities/vulnerabilities
- Condition immunities
- Languages

### Medium Priority

1. **Enhanced Template System**

- More pre-defined templates (Dragon, Fiend, Fey, etc.)
- Template chaining (apply multiple templates)
- Custom template creation in UI
1. **Search and Filter**

- Search monsters by name
- Filter by CR range
- Filter by stat values
1. **Export/Import**

- Export monsters to PDF or text
- Import from other D&D tools
- Bulk import/export

### Low Priority

1. **Combat Features**

- Initiative tracker
- Combat round management
- HP tracking during combat
1. **UI Improvements**

- Monster images/artwork
- Rich text formatting for abilities
- Dark/light theme toggle
1. **Advanced Features**

- Monster generation (random stat generation)
- CR calculator
- Encounter builder
- Party management

---

## 📚 Appendix

### D&D 5e Ability Score Ranges

| Score | Modifier | Descriptor    |
| ----- | -------- | ------------- |
| 1     | -5       | Very Weak     |
| 2-3   | -4       | Weak          |
| 4-5   | -3       | Below Average |
| 6-7   | -2       | Poor          |
| 8-9   | -1       | Average       |
| 10-11 | +0       | Normal        |
| 12-13 | +1       | Good          |
| 14-15 | +2       | Above Average |
| 16-17 | +3       | Strong        |
| 18-19 | +4       | Very Strong   |
| 20    | +5       | Exceptional   |
| 21-22 | +5 to +6 | Legendary     |
| 23-24 | +6 to +7 | Mythic        |
| 25+   | +7+      | Godlike       |

### Challenge Rating (CR) Guide

| CR  | XP     | Example Creatures  |
| --- | ------ | ------------------ |
| 0   | 0-9    | Commoner, Rat      |
| 1/8 | 25     | Goblin, Kobold     |
| 1/4 | 50     | Wolf, Skeletons    |
| 1/2 | 100    | Black Bear, Ogre   |
| 1   | 200    | Ghoul, Bugbear     |
| 2   | 450    | Ogre, Giant Spider |
| 5   | 1800   | Troll, Basilisk    |
| 10  | 5900   | Young Red Dragon   |
| 20  | 25000  | Ancient Red Dragon |
| 30  | 155000 | Tarrasque          |

---

## 📞 Support

This README was generated based on the codebase at: [https://github.com/m0rgi0n999/DndMonsterManager](https://github.com/m0rgi0n999/DndMonsterManager)

For questions or issues, please refer to the GitHub repository.

---

*Documentation generated on June 24, 2026*  
*For Erwin Rijvers*
