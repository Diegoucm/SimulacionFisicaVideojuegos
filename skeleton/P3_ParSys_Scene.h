#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "GravityGenerator.h"
#include "WindGenerator.h"

class P3_ParSys_Scene : public Scene {
public:
    explicit P3_ParSys_Scene(std::string name) : Scene(std::move(name)) {}

    void init() override {
        m_gravOnly.setBounds(physx::PxVec3(-150, -5, -150), physx::PxVec3(150, 120, 150));
        m_windy.setBounds(physx::PxVec3(-150, -5, -150), physx::PxVec3(150, 120, 150));

        const Vector3D baseGrav(-20, 0, 20);   // x + z constante para ver ambos a la misma profundidad
        const Vector3D baseWindy(20, 0, -20);

        // Sistema 1: SOLO gravedad
        m_gravOnly.addGenerator(makeJet(baseGrav, Vector4(0.2f, 0.5f, 1.0f, 1.0f)));
        m_gravOnly.addGenerator(makeSpray(baseGrav, Vector4(0.4f, 0.8f, 1.0f, 1.0f)));
        m_gravOnly.addForceGenerator(new GravityGenerator(Vector3D(0, -9.8f, 0)));

        // Sistema 2: gravedad + viento (mismos emisores, otro color)
        m_windy.addGenerator(makeJet(baseWindy, Vector4(1.0f, 0.4f, 0.1f, 1.0f)));
        m_windy.addGenerator(makeSpray(baseWindy, Vector4(1.0f, 0.8f, 0.2f, 1.0f)));
        m_windy.addForceGenerator(new GravityGenerator(Vector3D(0, -9.8f, 0)));

        // 20 m/s hacia +X dentro de una esfera grande que cubre toda la trayectoria
        m_wind = new WindGenerator(Vector3D(20, 0, 0), Vector3D(35, 25, -20), 60.f, 2.0f, 0.0f);
        m_windy.addForceGenerator(m_wind);

        // Segundo viento: ráfaga hacia +Z en una esfera más pequeña, en la parte alta de la trayectoria
        m_wind2 = new WindGenerator(Vector3D(0, 0, 20), Vector3D(35, 40, -20), 30.f, 2.0f, 0.0f);
        m_windy.addForceGenerator(m_wind2);
    }

    void update(double dt) override {
        m_gravOnly.update(dt);
        m_windy.update(dt);
    }

    // v: activar/desactivar el viento
    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        if ((key == 't' || key == 'T') && m_wind) m_wind->toggle();

        if ((key == 'y' || key == 'Y') && m_wind2) m_wind2->toggle();
    }

    void cleanup() override {
        m_gravOnly.clear();
        m_windy.clear();      // borra también m_wind (es suyo)
        m_wind = nullptr;
        m_wind2 = nullptr;
    }

private:
    // Chorro: estrecho, rápido, partículas "pesadas"
    static EmitterConfig makeJet(Vector3D base, Vector4 color) {
        EmitterConfig c;
        c.position = base;
        c.posSpread = Vector3D(1, 0, 1);
        c.dist = VelDistribution::Normal;
        c.velMean = Vector3D(0, 35, 0);
        c.velSpread = Vector3D(1.5f, 3, 1.5f);
        c.acc = Vector3D(0, 0, 0);       // OJO: el valor por defecto es (0,-10,0) y duplicaría la gravedad
        c.massMin = 2.f;  c.massMax = 4.f;
        c.radius = 0.3f;
        c.color = color;
        c.lifeTime = 8.f;
        c.rate = 150.f;
        return c;
    }

    // Rociada: ancha, más lenta, partículas ligeras
    static EmitterConfig makeSpray(Vector3D base, Vector4 color) {
        EmitterConfig c;
        c.position = base;
        c.posSpread = Vector3D(2, 0, 2);
        c.dist = VelDistribution::Normal;
        c.velMean = Vector3D(0, 25, 0);
        c.velSpread = Vector3D(6, 4, 6);
        c.acc = Vector3D(0, 0, 0);
        c.massMin = 0.5f;  c.massMax = 2.f;
        c.radius = 0.2f;
        c.color = color;
        c.lifeTime = 6.f;
        c.rate = 150.f;
        return c;
    }

    ParticleSystem m_gravOnly, m_windy;
    WindGenerator* m_wind = nullptr;   // sin ownership: lo posee m_windy
    WindGenerator* m_wind2 = nullptr;   // sin ownership: lo posee m_windy
};