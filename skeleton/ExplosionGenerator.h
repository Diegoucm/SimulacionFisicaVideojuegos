#pragma once
#include <algorithm>
#include <cmath>
#include "ForceGenerator.h"
#include "Particle.h"

class ExplosionGenerator : public ForceGenerator {
public:
    // K: intensidad, radius: radio (máximo si hay onda expansiva), tau: constante de tiempo
    // expansionSpeed > 0 activa la onda expansiva: R(t) = min(radius, v_e * t)
    ExplosionGenerator(Vector3D center, float K, float radius, float tau,
        float expansionSpeed = 0.0f)
        : m_center(center), m_K(K), m_radius(radius), m_tau(tau), m_ve(expansionSpeed) {
        setActive(false);                       // dormida hasta detonate()
    }

    void detonate() { m_t = 0.0; setActive(true); }

    // llamar UNA vez por frame, antes de acumular fuerzas
    void update(double dt) {
        if (!isActive()) return;
        m_t += dt;
        if (m_t > 4.0 * m_tau) setActive(false);   // a partir de 4*tau ya se ha desvanecido
    }

    void updateForce(Particle* p, double dt) override {
        if (p->getInvMass() <= 0.0f) return;

        Vector3D pos = p->getPosition().p;
        Vector3D d = pos - m_center;
        float r = d.magnitude();

        float R = (m_ve > 0.0f) ? (std::min)(m_radius, m_ve * (float)m_t) : m_radius;
        if (r >= R) return;

        float r2 = (std::max)(r * r, 1.0f);           // evita la singularidad en el centro
        float k = (m_K / r2) * std::exp(-(float)(m_t / m_tau));
        p->addForce(d * k);
    }

    void setExpansionSpeed(float ve) { m_ve = ve; }
    void setCenter(Vector3D c) { m_center = c; }

private:
    Vector3D m_center;
    float m_K, m_radius, m_tau, m_ve;
    double m_t = 0.0;
};