#pragma once
#include "Vector3D.h"
#include "PxPhysicsAPI.h"
#include "RenderUtils.hpp"

enum class TipoIntegracion { Euler, EulerSemiExplic, Verlet };

class Particle
{
public:
	Particle(Vector3D p, Vector3D v, Vector3D a, float damp, 
			 float radius = 1.0f, Vector4 color = Vector4(1.0f, 0.0f, 0.0f, 1.0f)); 
	// por ahora metemos la aceleracion aque pero luego lo quitaremos y la acel solo cambiara con fuerzas que se apliquen. 
	// He añadido el radio y color con valores por defecto para poder cambiarlo si quiero crear particulas con distinto tamaño y color

	virtual ~Particle();

	void integrate(double t, TipoIntegracion tipoIntegr);


	// GETTERS
	physx::PxTransform getPosition() const { return pos; }

	float getMass() const { return 1.0f / w; } // inversa

	float getInvMass() const { return w; }

	Vector3D getVel() const { return vel; }

	bool  isExpired() const { return lifeTime >= 0.0f && age >= lifeTime; }

	// SETTERS
	void setMass(float mass) { w = 1.0f / mass; }

	void setVel(Vector3D v) { vel = v; }

	void setAcc(Vector3D a) { acc = a; }

	void  setLifeTime(float s) { lifeTime = s; }

	void addForce(Vector3D f) { force += f; }

protected:
	Vector3D vel;
	physx::PxTransform pos;
	Vector3D force = Vector3D(0, 0, 0);   // acumulador de fuerzas
	RenderItem* renderItem;

	float w; // inversa de la masa, puede que lo tenga que cambiar a double no estoy seguro
	float damping;

	float lifeTime = -1; // negativo es infinito, positivo son los segundos de vida
	float age = 0; // edad de la particula, para ver si se ha pasado de lifeTime

	// por ahora guardamos aceleracion aunque luego no hara falta cuando hagamos con fuerzas
	Vector3D acc;
};

