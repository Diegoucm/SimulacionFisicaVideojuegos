#include "SpringForceGenerator.h"
#include "Vector3D.h"
#include "Particle.h"

void SpringForceGenerator::updateForce(Particle* p, double dt)
{
	Vector3 relativePos = _other->getPosition().p - p->getPosition().p;
	Vector3 force;

	const float length = relativePos.normalize();
	const float delta_x = length - _restingLength;

	force = relativePos * delta_x * _k;

	p->addForce(force);

}