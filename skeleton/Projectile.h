#pragma once

#include "Particle.h"

class Projectile : public Particle
{
public:
    Projectile(Vector3D pos, Vector3D dir, float realMass, float realSpeed,
        float simSpeed, float realGravity, float damp);

    float getRealMass() const { return realMass; }
    float getSimMass()  const { return getMass(); }

    void setRealMass(float m) { realMass = m;    updateSimulated(); }
    void setRealGravity(float g) { realGravity = g; updateSimulated(); }
    void setRealSpeed(float s) { realSpeed = s;   updateSimulated(); }
    void setSimSpeed(float s);   // además reescala la velocidad actual

private:
    void updateSimulated();      // masa y gravedad simuladas

    float realMass;
    float realSpeed;
    float simSpeed;
    float realGravity;
};