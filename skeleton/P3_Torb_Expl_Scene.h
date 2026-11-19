#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "GravityGenerator.h"
#include "WhirlwindGenerator.h"
#include "ExplosionGenerator.h"

class P3_Torb_Expl_Scene : public Scene {
public:
    explicit P3_Torb_Expl_Scene(std::string name) : Scene(std::move(name)) {}

    void init() override {
        // x + z constante en los dos puntos para verlos a la misma profundidad
        // ---------------- TORBELLINO ----------------
        const Vector3D vortexCenter(-30, 0, 30);

        EmitterConfig dust;
        dust.position = Vector3D(-30, 1, 30);
        dust.posSpread = Vector3D(15, 0, 15);
        dust.velMean = Vector3D(0, 0, 0);
        dust.velSpread = Vector3D(0, 0, 0);
        dust.acc = Vector3D(0, 0, 0);       // el valor por defecto es (0,-10,0): duplicaría la gravedad
        dust.massMin = 0.2f;  dust.massMax = 1.0f;
        dust.radius = 0.25f;
        dust.color = Vector4(0.8f, 0.7f, 0.4f, 1.0f);
        dust.lifeTime = 10.f;
        dust.rate = 200.f;
        m_vortexSys.addGenerator(dust);

        m_vortexSys.addForceGenerator(new GravityGenerator(Vector3D(0, -9.8f, 0)));
        m_vortex = new WhirlwindGenerator(vortexCenter, 0.8f, 67.f, 1.0f);   // K, radio, k1
        m_vortexSys.addForceGenerator(m_vortex);

        // ---------------- EXPLOSION ----------------
        const Vector3D boomCenter(30, 30, -30);

        EmitterConfig cloud;
        cloud.position = boomCenter;
        cloud.posSpread = Vector3D(12, 12, 12);
        cloud.velMean = Vector3D(0, 0, 0);
        cloud.velSpread = Vector3D(0, 0, 0);
        cloud.acc = Vector3D(0, 0, 0);
        cloud.massMin = 1.f;  cloud.massMax = 20.f;
        cloud.radius = 0.5f;
        cloud.color = Vector4(1.0f, 0.3f, 0.3f, 1.0f);
        cloud.lifeTime = 20.f;
        cloud.rate = 25.f;
        m_boomSys.addGenerator(cloud);

        m_explosion = new ExplosionGenerator(boomCenter, 5000.f, 50.f, 0.67f);  // K, R, tau
        m_boomSys.addForceGenerator(m_explosion);
    }

    void update(double dt) override {
        m_explosion->update(dt);     // avanzar el tiempo de la explosión ANTES de acumular fuerzas
        m_vortexSys.update(dt);
        m_boomSys.update(dt);
    }

    // e: explosión | x: activar/desactivar onda expansiva | o: torbellino on/off
    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        if ((key == 'e' || key == 'E') && m_explosion) m_explosion->detonate();
        if ((key == 'o' || key == 'O') && m_vortex)    m_vortex->toggle();
        if ((key == 'x' || key == 'X') && m_explosion) {
            m_shockwave = !m_shockwave;
            m_explosion->setExpansionSpeed(m_shockwave ? 60.f : 0.f);
        }
    }

    void cleanup() override {
        m_vortexSys.clear();     // borra también m_vortex
        m_boomSys.clear();       // borra también m_explosion
        m_vortex = nullptr;
        m_explosion = nullptr;
    }

private:
    ParticleSystem m_vortexSys, m_boomSys;
    WhirlwindGenerator* m_vortex = nullptr;     // sin ownership: lo posee m_vortexSys
    ExplosionGenerator* m_explosion = nullptr;  // sin ownership: lo posee m_boomSys
    bool m_shockwave = false;
};