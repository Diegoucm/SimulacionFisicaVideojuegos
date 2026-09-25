#pragma once
#include "Vector3D.h"
#include "PxPhysicsAPI.h"
#include "RenderUtils.hpp"

enum class TipoIntegracion { Euler, EulerSemiExplic, Verlet };

class Particle
{
public:
	Particle(Vector3D p, Vector3D v, Vector3D a, float damp); // por ahora metemos la aceleracion aque pero luego lo quitaremos y la acel solo cambiara con fuerzas que se apliquen.

	~Particle();

	void integrate(double t, TipoIntegracion tipoIntegr);

	physx::PxTransform getPosition() const { return pos; }

private:
	Vector3D vel;
	physx::PxTransform pos;
	RenderItem* renderItem;

	float w; // inversa de la masa, puede que lo tenga que cambiar a double no estoy seguro
	float damping;


	// por ahora guardamos aceleracion aunque luego no hara falta
	Vector3D acc;



};

