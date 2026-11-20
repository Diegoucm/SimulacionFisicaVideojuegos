#pragma once

#include "SpringForceGenerator.h"
#include "Vector3D.h"

class AnchoredSpring : public SpringForceGenerator
{
public:
	AnchoredSpring(double k, double restingLength, const Vector3D& anchor);

	~AnchoredSpring();

};

