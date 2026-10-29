#pragma once
#include "Scene.h"
#include <vector>
#include "Particle.h"
#include "GravityGenerator.h"
#include "ForceRegistry.h"
#include <cstdio>

class P3_Scene : public Scene {
public:
    explicit P3_Scene(std::string name) : Scene(std::move(name)) {}

    void init() override {
        m_gravity = new GravityGenerator(Vector3D(0, -9.8f, 0));
        m_gravityFloat = new GravityGenerator(Vector3D(0, 2.0f, 0));  // globo

        float masses[3] = { 1.f, 5.f, 20.f };
        for (int i = 0; i < 3; ++i) {
            float z = i * 5.f;

            // A) método viejo: aceleración constante, sin generador
            Particle* a = new Particle(Vector3D(-15 + i * 4.f, 50, 15 - i * 4.f), Vector3D(0, 0, 0),
                Vector3D(0, -9.8f, 0), 0.99f, 1.f,
                Vector4(1, 0, 0, 1));
            a->setMass(masses[i]);
            m_particles.push_back(a);

            // B) con generador gravitatorio (acc = 0, solo fuerzas)
            Particle* b = new Particle(Vector3D(5 + i * 4.f, 50, -5 - i * 4.f), Vector3D(0, 0, 0),
                Vector3D(0, 0, 0), 0.99f, 1.f,
                Vector4(0, 1, 0, 1));
            b->setMass(masses[i]);
            m_particles.push_back(b);
            m_registry.add(b, m_gravity);

            // C) globo: segunda instancia del generador
            Particle* c = new Particle(Vector3D(25 + i * 4.f, 10, -25 - i * 4.f), Vector3D(0, 0, 0),
                Vector3D(0, 0, 0), 0.99f, 1.f,
                Vector4(0, 0, 1, 1));
            c->setMass(masses[i]);
            m_particles.push_back(c);
            m_registry.add(c, m_gravityFloat);
        }
    }

    void update(double dt) override {
        m_registry.update(dt);                       // 1) acumular fuerzas
        for (Particle* p : m_particles)              // 2) integrar (limpia el acumulador)
            p->integrate(dt, TipoIntegracion::EulerSemiExplic);

        m_time += dt;
        if (!m_printed && m_time >= 2.0) {
            m_printed = true;
            for (size_t i = 0; i < m_particles.size(); ++i)
                printf("particula %zu  masa=%.1f  y=%.4f\n", i,
                    m_particles[i]->getMass(), m_particles[i]->getPosition().p.y);
        }
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {}

    void cleanup() override {
        m_registry.clear();
        for (Particle* p : m_particles) delete p;
        m_particles.clear();
        delete m_gravity;      m_gravity = nullptr;
        delete m_gravityFloat; m_gravityFloat = nullptr;
    }

private:
    std::vector<Particle*> m_particles;
    ForceRegistry m_registry;
    GravityGenerator* m_gravity = nullptr;
    GravityGenerator* m_gravityFloat = nullptr;

    double m_time = 0.0;
    bool m_printed = false;
};
