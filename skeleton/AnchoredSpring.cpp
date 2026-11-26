#include "AnchoredSpring.h"
#include "Particle.h"

AnchoredSpring::AnchoredSpring(double k, double restingLength, const Vector3D& anchor): SpringForceGenerator(k, restingLength, nullptr) {

	_other = new Particle(anchor, Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 0.5f, Vector4(1.0f, 0.0f, 1.0f, 1.0f), ParticleShape::Box);
}

AnchoredSpring::~AnchoredSpring() {
	if (_other) {
		delete _other;
		_other = nullptr;
	}
}