#include "LiquidDragForceGenerator.h"
#include "BuoyancyForceGenerator.h"
#include "Particle.h"

LiquidDragForceGenerator::LiquidDragForceGenerator(float k1, float k2, float height, float surfaceY,
	const physx::PxVec3& fluidVel)
	: _k1(k1), _k2(k2), _height(height), _surfaceY(surfaceY), _fluidVel(fluidVel) {
}

void LiquidDragForceGenerator::updateForce(Particle* p, double dt) {
	if (!p) return;

	const float immersed = BuoyancyForceGenerator::immersedFraction(p->getPosition().p.y, _surfaceY, _height);
	if (immersed <= 0.0f) return;

	Vector3D v = p->getVel();
	const physx::PxVec3 vRel = _fluidVel - physx::PxVec3((float)v.x, (float)v.y, (float)v.z);
	const float speed = vRel.magnitude();
	if (speed < 1e-6f) return;

	physx::PxVec3 f = vRel * (_k1 + _k2 * speed) * immersed;

	// Seguridad numerica: con coeficientes grandes y masas pequenas el integrador discreto haria que
	// el rozamiento "pasara de largo" y metiera energia (k1*dt/m > 2). Se limita la fuerza para que, como
	// mucho, anule la velocidad relativa en un paso.
	const float dtf = (float)dt;
	if (dtf > 0.0f) {
		const float fMax = speed * p->getMass() / dtf;
		const float fMag = f.magnitude();
		if (fMag > fMax) f *= (fMax / fMag);
	}

	p->addForce(Vector3D(f.x, f.y, f.z));
}