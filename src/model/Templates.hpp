#pragma once
#include "Monster.hpp"

struct UndeadTemplate {
    void apply(Monster& m) {
        m.hp += 10;
        m.abilities.push_back("Undead Fortitude");
    }
};

// Use this to apply any template that satisfies our concept
template<typename T>
void applyTemplate(Monster& m, T templateObj) {
    templateObj.apply(m);
}