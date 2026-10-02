#include "Projectile.h"

Projectile::Projectile(Vector3D pos, Vector3D dir, float rMass, float rSpeed,
    float sSpeed, float rGravity, float damp)
    : Particle(pos, dir* sSpeed, Vector3D(0, 0, 0), damp),  // dir ya normalizada
    realMass(rMass), realSpeed(rSpeed), simSpeed(sSpeed), realGravity(rGravity)
{
    updateSimulated();
}

void Projectile::updateSimulated()
{
    float ratio = realSpeed / simSpeed;
    float k2 = ratio * ratio;

    setMass(realMass * k2);                    // misma energía cinética
    setAcc(Vector3D(0, realGravity / k2, 0));  // g_s = g_r * (v_s/v_r)^2
}

void Projectile::setSimSpeed(float s)
{
    vel *= (s / simSpeed);   // mantiene la dirección, cambia el módulo
    simSpeed = s;
    updateSimulated();
}