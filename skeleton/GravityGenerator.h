#pragma once
#include "ForceGenerator.h"
#include "Particle.h"

class GravityGenerator : public ForceGenerator {
public:
    explicit GravityGenerator(Vector3D g) : gravity(g) {}

    void updateForce(Particle* p, double dt) override {
        if (p->getInvMass() <= 0.0f) return;     // masa infinita: no se mueve
        p->addForce(gravity * p->getMass());     // F = m * g
    }

    void setGravity(Vector3D g) { gravity = g; }

private:
    Vector3D gravity;
};