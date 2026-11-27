#pragma once

#include "ForceGenerator.h"
#include "PxPhysicsAPI.h"

// Rozamiento viscoso con el liquido (mismo modelo que el generador de viento):
//     F = immersed * ( k1 * (vf - v) + k2 * |vf - v| * (vf - v) )
// vf = velocidad del fluido (0 = liquido en reposo), v = velocidad de la particula.
//  - k1 domina en liquidos (cuanto mas viscoso, mayor k1) -> k2 = 0 por defecto.
//  - k2 domina en gases.
// Se multiplica por la fraccion sumergida: fuera del liquido no hay rozamiento.
class LiquidDragForceGenerator : public ForceGenerator
{
public:
	LiquidDragForceGenerator(float k1, float k2, float height, float surfaceY,
		const physx::PxVec3& fluidVel = physx::PxVec3(0.f));

	void updateForce(Particle* p, double dt) override;

	~LiquidDragForceGenerator() override = default;

	void setK1(float k1) { _k1 = k1; }
	void setK2(float k2) { _k2 = k2; }
	void setHeight(float h) { _height = h; }
	void setSurfaceY(float y) { _surfaceY = y; }
	void setFluidVelocity(const physx::PxVec3& v) { _fluidVel = v; }

	float getK1() const { return _k1; }
	float getK2() const { return _k2; }

protected:
	float _k1;
	float _k2;
	float _height;         // arista del cubo (para calcular la fraccion sumergida)
	float _surfaceY;
	physx::PxVec3 _fluidVel;
};