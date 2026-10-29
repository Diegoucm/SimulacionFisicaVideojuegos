#pragma once
#include <vector>
#include <algorithm>
#include "ForceGenerator.h"

class ForceRegistry {
public:
    void add(Particle* p, ForceGenerator* g) { m_entries.push_back({ p, g }); }

    void remove(Particle* p, ForceGenerator* g) {
        m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(),
            [&](const Entry& e) { return e.particle == p && e.gen == g; }),
            m_entries.end());
    }

    // llamar SIEMPRE antes de hacer delete de una partícula
    void removeParticle(Particle* p) {
        m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(),
            [&](const Entry& e) { return e.particle == p; }),
            m_entries.end());
    }

    void update(double dt) {
        for (auto& e : m_entries) e.gen->updateForce(e.particle, dt);
    }

    void clear() { m_entries.clear(); }

private:
    struct Entry { Particle* particle; ForceGenerator* gen; };
    std::vector<Entry> m_entries;
};