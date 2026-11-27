#include "SpringForceGenerator.h"
#include "Vector3D.h"
#include "Particle.h"

void SpringForceGenerator::updateForce(Particle* p, double dt)
{
	Vector3 relativePos = _other->getPosition().p - p->getPosition().p;
	Vector3 force;

	const float length = relativePos.normalize();
	const float delta_x = length - _restingLength;

	force = relativePos * delta_x * _k;          // Hooke: -k x (apunta hacia el otro extremo)

	// Amortiguamiento: Fa = -b * (v - v_other). Con el muelle anclado, el anclaje no se mueve (v_other = 0).
	if (_b != 0.0) {
		Vector3 relVel = p->getVel().toPxVec3() - _other->getVel().toPxVec3();
		Vector3 damp = relVel * (float)_b;

		// Seguridad numerica: con b grande y masa pequena (b*dt/m > 2) el integrador discreto metería energia.
		// Se limita para que, como mucho, anule la velocidad relativa en un paso.
		const float dtf = (float)dt;
		if (dtf > 0.0f) {
			const float maxMag = relVel.magnitude() * p->getMass() / dtf;
			const float mag = damp.magnitude();
			if (mag > maxMag) damp *= (maxMag / mag);
		}
		force -= damp;
	}

	p->addForce(force);
}