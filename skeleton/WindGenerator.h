#pragma once
#include "ForceGenerator.h"
#include "Particle.h"

// F = k1*(vw - v) + k2*|vw - v|*(vw - v)  =  (vw - v) * (k1 + k2*|vw - v|)
class WindGenerator : public ForceGenerator {
public:
    WindGenerator(Vector3D windVel, Vector3D center, float radius,
        float k1, float k2 = 0.0f)
        : m_wind(windVel), m_center(center), m_radius(radius), m_k1(k1), m_k2(k2) {
    }

    void updateForce(Particle* p, double dt) override {
        if (p->getInvMass() <= 0.0f) return;              // masa infinita: no se mueve

        Vector3D pos = p->getPosition().p;                // PxVec3 -> Vector3D (constructor)
        if (!isInside(pos)) return;                       // fuera del volumen: sin viento

        Vector3D diff = getWindVelocity(pos) - p->getVel();   // velocidad relativa
        p->addForce(diff * (m_k1 + m_k2 * diff.magnitude()));
    }

    void setWind(Vector3D w) { m_wind = w; }
    void setK1(float k) { m_k1 = k; }
    void setK2(float k) { m_k2 = k; }

protected:
    // Para el torbellino (actividad 3) solo hay que sobrescribir esto
    virtual Vector3D getWindVelocity(const Vector3D& pos) const { return m_wind; }

    // Volumen de influencia: esfera
    virtual bool isInside(const Vector3D& pos) const {
        return (pos - m_center).magnitude() <= m_radius;
    }

    Vector3D m_wind;
    Vector3D m_center;
    float m_radius;
    float m_k1, m_k2;
};