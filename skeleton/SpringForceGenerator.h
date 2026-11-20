#pragma once

#include "ForceGenerator.h"

class SpringForceGenerator: public ForceGenerator
{
public:
	SpringForceGenerator(double k, double restingLength, Particle* other)
		: _k(k), _restingLength(restingLength), _other(other) {
	}

	virtual void updateForce(Particle* p, double dt) override;

	inline void setK(double k) { _k = k; }

	virtual ~SpringForceGenerator() {};

protected:
	double _k;
	double _restingLength;
	Particle* _other;
};

