# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(DndMonsterManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Auto-MOC/RCC/UIC
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(nlohmann_json REQUIRED)

# 1. Define the executable first
add_executable(DndMonsterManager 
  src/main.cpp 
  src/model/Monster.cpp 
  src/gui/MainWindow.cpp
)

# 2. Now you can safely reference the target
target_include_directories(DndMonsterManager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(DndMonsterManager PRIVATE Qt6::Widgets nlohmann_json::nlohmann_json)
