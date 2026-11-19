#pragma once
#include "WindGenerator.h"

// v(x,y,z) = K * [ -(z - zc),  updraft - (y - yc),  (x - xc) ]
// updraft = 50 es el valor del enunciado; con 0 queda un remolino puramente horizontal
class WhirlwindGenerator : public WindGenerator {
public:
    WhirlwindGenerator(Vector3D center, float K, float radius,
        float k1, float k2 = 0.0f, float updraft = 50.0f)
        : WindGenerator(Vector3D(0, 0, 0), center, radius, k1, k2),
        m_K(K), m_updraft(updraft) {
    }

    void setK(float K) { m_K = K; }

protected:
    Vector3D getWindVelocity(const Vector3D& pos) const override {
        Vector3D d = pos - m_center;
        return Vector3D(-d.z, m_updraft - d.y, d.x) * m_K;
    }

private:
    float m_K;
    float m_updraft;
};