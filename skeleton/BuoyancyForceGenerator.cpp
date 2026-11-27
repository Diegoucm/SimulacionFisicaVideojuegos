#include "BuoyancyForceGenerator.h"
#include "Particle.h"

BuoyancyForceGenerator::BuoyancyForceGenerator(float height, float volume, float liquidDensity, float surfaceY, float gravity)
	: _height(height), _volume(volume), _liquidDensity(liquidDensity), _surfaceY(surfaceY), _gravity(gravity) {
}

float BuoyancyForceGenerator::immersedFraction(float h, float h0, float height) {
	if (height <= 0.0f) return 0.0f;
	if (h - h0 > height * 0.5f)  return 0.0f;                 // completamente fuera
	if (h0 - h > height * 0.5f)  return 1.0f;                 // completamente sumergido
	return (h0 - h) / height + 0.5f;                          // parcial: profundidad normalizada
}

void BuoyancyForceGenerator::updateForce(Particle* p, double /*dt*/) {
	if (!p) return;

	const float immersed = immersedFraction(p->getPosition().p.y, _surfaceY, _height);

	const float fy = _liquidDensity * _volume * immersed * _gravity;
	p->addForce(Vector3D(0.0f, fy, 0.0f));
}