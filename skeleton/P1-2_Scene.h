// P1-2_Scene.h
#pragma once
#include "Scene.h"
#include <vector>
#include <cctype>
#include "Particle.h"
#include "Projectile.h"

class P1_2_Scene : public Scene {
public:
    explicit P1_2_Scene(std::string name) : Scene(std::move(name)) {};

    void init() override {
        //            tecla  masaReal velReal velSim gravReal damping
        m_types = {
            { '1',   0.01f,  250.f,  25.f,  -9.8f,  0.99f },   // bala
            { '2',   5.0f,   300.f,  30.f,  -9.8f,  0.99f },   // cañonazo
            { '3',   0.05f,  10.f,   10.f,   2.0f,  0.98f },   // globo (gravedad +, flota)
        };
    }

    void update(double dt) override {
        for (Particle* p : m_particles)
            p->integrate(dt, TipoIntegracion::EulerSemiExplic);
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        key = std::tolower(key);

        // disparo: una tecla por tipo
        for (int i = 0; i < (int)m_types.size(); ++i) {
            if (key == m_types[i].key) {
                m_selected = i;
                fire(m_types[i], camera);
                return;
            }
        }

        // cambiar masa real del tipo seleccionado
        if (m_selected < 0) return;
        ProjectileType& t = m_types[m_selected];
        if (key == '+') {
            t.realMass *= 1.2f;
        }
        else if (key == '-') {
            t.realMass /= 1.2f;
        }
        else return;

        // aplicar también a los ya lanzados de ese tipo
        for (Projectile* p : m_projectiles[m_selected])
            p->setRealMass(t.realMass);
    }

    void cleanup() override {
        for (Particle* p : m_particles) delete p;
        m_particles.clear();
        for (auto& v : m_projectiles) v.clear();
    }

private:
    struct ProjectileType {
        unsigned char key;
        float realMass, realSpeed, simSpeed, realGravity, damping;
    };

    void fire(const ProjectileType& t, const physx::PxTransform& cam) {
        physx::PxVec3 d = cam.q.rotate(physx::PxVec3(0, 0, -1)).getNormalized();

        Projectile* p = new Projectile(
            Vector3D(cam.p.x, cam.p.y, cam.p.z),
            Vector3D(d.x, d.y, d.z),
            t.realMass, t.realSpeed, t.simSpeed, t.realGravity, t.damping);

        m_particles.push_back(p);
        m_projectiles[m_selected].push_back(p);   // puntero "tipado" para poder cambiarle la masa
    }

    std::vector<Particle*> m_particles;                 // dueño de la memoria
    std::vector<ProjectileType> m_types;
    std::vector<Projectile*> m_projectiles[3];          // sin ownership, uno por tipo
    int m_selected = -1;
};