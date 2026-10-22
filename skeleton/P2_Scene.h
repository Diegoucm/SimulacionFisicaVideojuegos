#pragma once
#include "Scene.h"
#include "ParticleSystem.h"

class P2_Scene : public Scene {
public:
    explicit P2_Scene(std::string name) : Scene(std::move(name)) {};

    void init() override {
        m_system.setBounds(physx::PxVec3(-100, -5, -100), physx::PxVec3(100, 100, 100));

        // ---------- 1) FUENTE / MANGUERA ----------
        // Punto fijo, velocidad gaussiana alrededor de "hacia arriba", con gravedad -10.
        EmitterConfig fountain;
        fountain.position = Vector3D(0, 0, 0);
        fountain.posSpread = Vector3D(0, 0, 0);
        fountain.dist = VelDistribution::Normal;
        fountain.velMean = Vector3D(0, 30, 0);
        fountain.velSpread = Vector3D(4, 3, 4);          // sigma por componente
        fountain.acc = Vector3D(0, -10.0f, 0);
        fountain.radius = 0.3f;
        fountain.color = Vector4(0.2f, 0.5f, 1.0f, 1.0f);
        fountain.lifeTime = 7.0f;
        fountain.rate = 80.0f;
        m_system.addGenerator(fountain);

        // ---------- 2) NIEBLA ----------
        // Posicion muy variable (caja grande), velocidades pequenyas y uniformes,
        // particulas pequenyas y muy transparentes
        EmitterConfig fog;
        fog.position = Vector3D(40, 4, 0);
        fog.posSpread = Vector3D(20, 3, 20);
        fog.dist = VelDistribution::Uniform;
        fog.velMean = Vector3D(0, 0, 0);
        fog.velSpread = Vector3D(0.8f, 0.3f, 0.8f);      // semi-rango
        fog.acc = Vector3D(0, -10.0f, 0);
        fog.radius = 0.25f;
        fog.color = Vector4(0.85f, 0.85f, 0.9f, 0.15f);
        fog.lifeTime = 8.0f;
        fog.rate = 60.0f;
        m_system.addGenerator(fog);

        // ---------- 3) FUEGO ----------
        // Punto casi fijo, sube (aceleracion hacia arriba = empuje del aire caliente),
        // vida corta y color calido semitransparente.
        EmitterConfig fire;
        fire.position = Vector3D(-40, 0, 0);
        fire.posSpread = Vector3D(1.0f, 0, 1.0f);
        fire.dist = VelDistribution::Normal;
        fire.velMean = Vector3D(0, 15, 0);
        fire.velSpread = Vector3D(1.5f, 1.5f, 1.5f);
        fire.acc = Vector3D(0, -10.0f, 0);
        fire.damping = 0.9f;
        fire.radius = 0.4f;
        fire.color = Vector4(1.0f, 0.4f, 0.05f, 0.6f);
        fire.lifeTime = 2.0f;
        fire.rate = 100.0f;
        m_system.addGenerator(fire);
    }

    void update(double dt) override {
        m_system.update(dt);
    }

    // 1, 2, 3: activar/desactivar cada emisor
    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        if (key >= '1' && key <= '3') {
            if (ParticleGenerator* g = m_system.getGenerator(key - '1'))
                g->toggle();
        }
    }

    void cleanup() override {
        m_system.clear();   // libera particulas y generadores
    }

private:
    ParticleSystem m_system;
};
