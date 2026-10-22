#pragma once
#include <vector>
#include "Particle.h"
#include "ParticleGenerator.h"

class ParticleSystem {
public:
    ParticleSystem() = default;
    ~ParticleSystem() { clear(); }

    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;

    // Espacio de accion: fuera de esta caja la particula se elimina.
    void setBounds(const physx::PxVec3& mn, const physx::PxVec3& mx) { bmin = mn; bmax = mx; }

    ParticleGenerator* addGenerator(const EmitterConfig& cfg) {
        m_generators.push_back(new ParticleGenerator(cfg));
        return m_generators.back();
    }

    ParticleGenerator* getGenerator(size_t i) {
        return i < m_generators.size() ? m_generators[i] : nullptr;
    }

    void update(double dt) {
        // 1) los generadores crean particulas nuevas
        for (ParticleGenerator* g : m_generators)
            g->emit(dt, m_particles);

        // 2) integrar y eliminar las que mueren (swap-and-pop, sin mover todo el vector)
        size_t i = 0;
        while (i < m_particles.size()) {
            Particle* p = m_particles[i];
            p->integrate(dt, TipoIntegracion::EulerSemiExplic);

            if (p->isExpired() || outOfBounds(p->getPosition().p)) {
                delete p;                          // el destructor libera el RenderItem
                m_particles[i] = m_particles.back();
                m_particles.pop_back();            // no avanzamos i: hay una particula nueva en ese hueco
            }
            else {
                ++i;
            }
        }
    }

    void clear() {
        for (Particle* p : m_particles) delete p;
        m_particles.clear();
        for (ParticleGenerator* g : m_generators) delete g;
        m_generators.clear();
    }

    size_t particleCount() const { return m_particles.size(); }

private:
    bool outOfBounds(const physx::PxVec3& p) const {
        return p.x < bmin.x || p.y < bmin.y || p.z < bmin.z ||
               p.x > bmax.x || p.y > bmax.y || p.z > bmax.z;
    }

    std::vector<Particle*> m_particles;
    std::vector<ParticleGenerator*> m_generators;
    physx::PxVec3 bmin{ -100.f, -10.f, -100.f };
    physx::PxVec3 bmax{ 100.f, 100.f, 100.f };
};
