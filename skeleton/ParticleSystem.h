#pragma once
#include <vector>
#include "Particle.h"
#include "ParticleGenerator.h"
#include "ForceRegistry.h"

class ParticleSystem {
public:
    ParticleSystem() = default;
    ~ParticleSystem() { clear(); }

    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;

    void setBounds(const physx::PxVec3& mn, const physx::PxVec3& mx) { bmin = mn; bmax = mx; }

    ParticleGenerator* addGenerator(const EmitterConfig& cfg) {
        m_generators.push_back(new ParticleGenerator(cfg));
        return m_generators.back();
    }

    ParticleGenerator* getGenerator(size_t i) {
        return i < m_generators.size() ? m_generators[i] : nullptr;
    }

    // Generador de fuerzas global: se aplica a TODAS las particulas que nazcan.
    // El sistema pasa a ser su dueno (lo borra en clear()).
    void addForceGenerator(ForceGenerator* g) { m_forceGens.push_back(g); }

    void update(double dt) {
        // 1) los generadores crean particulas nuevas
        size_t before = m_particles.size();
        for (ParticleGenerator* g : m_generators)
            g->emit(dt, m_particles);

        // 2) registrar las recien creadas (estan en [before, size)) con cada generador de fuerzas
        for (size_t k = before; k < m_particles.size(); ++k)
            for (ForceGenerator* fg : m_forceGens)
                m_registry.add(m_particles[k], fg);

        // 3) acumular fuerzas ANTES de integrar
        m_registry.update(dt);

        // 4) integrar y eliminar las que mueren (swap-and-pop)
        size_t i = 0;
        while (i < m_particles.size()) {
            Particle* p = m_particles[i];
            p->integrate(dt, TipoIntegracion::EulerSemiExplic);

            if (p->isExpired() || outOfBounds(p->getPosition().p)) {
                m_registry.removeParticle(p);      // SIEMPRE antes del delete
                delete p;
                m_particles[i] = m_particles.back();
                m_particles.pop_back();
            }
            else {
                ++i;
            }
        }
    }

    void clear() {
        m_registry.clear();                        // primero el registro, luego las particulas
        for (Particle* p : m_particles) delete p;
        m_particles.clear();
        for (ParticleGenerator* g : m_generators) delete g;
        m_generators.clear();
        for (ForceGenerator* f : m_forceGens) delete f;
        m_forceGens.clear();
    }

    size_t particleCount() const { return m_particles.size(); }

private:
    bool outOfBounds(const physx::PxVec3& p) const {
        return p.x < bmin.x || p.y < bmin.y || p.z < bmin.z ||
            p.x > bmax.x || p.y > bmax.y || p.z > bmax.z;
    }

    std::vector<Particle*> m_particles;
    std::vector<ParticleGenerator*> m_generators;
    std::vector<ForceGenerator*> m_forceGens;
    ForceRegistry m_registry;
    physx::PxVec3 bmin{ -100.f, -5.f, -100.f };
    physx::PxVec3 bmax{ 100.f, 100.f, 100.f };
};