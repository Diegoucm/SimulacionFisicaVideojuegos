#pragma once

#include "ForceGenerator.h"

class SpringForceGenerator : public ForceGenerator
{
public:
	SpringForceGenerator(double k, double restingLength, Particle* other)
		: _k(k), _restingLength(restingLength), _other(other) {
	}

	virtual void updateForce(Particle* p, double dt) override;

	inline void setK(double k) { _k = k; }
	inline double getK() const { return _k; }

	// amortiguamiento: Fa = -b * v_rel   (b = 0 -> muelle ideal, sin perdidas)
	inline void setDamping(double b) { _b = b; }
	inline double getDamping() const { return _b; }

	virtual ~SpringForceGenerator() {};

protected:
	double _k;
	double _restingLength;
	Particle* _other;
	double _b = 0.0;      // constante de amortiguamiento
};