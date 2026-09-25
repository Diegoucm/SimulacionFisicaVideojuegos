#include "Particle.h"

Particle::Particle(Vector3D p, Vector3D v, Vector3D a, float damp) : pos(physx::PxVec3(p.x, p.y, p.z)), vel(v), acc(a), damping(damp) {
	physx::PxShape* shape = CreateShape(physx::PxSphereGeometry(1.0f));
	renderItem = new RenderItem(shape, &pos, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}


//por ahora se libera en el destructor dependiendo del tiempo que lleve viva la particula, 
// pero se puede hacer que se libere cuando salga de la pantalla o cuando colisione con algo mas adelante

Particle::~Particle() {
	if (renderItem) {
		renderItem->release();
		renderItem = nullptr;
	}
}

void Particle::integrate(double t, TipoIntegracion tipoIntegr) {
	// Empezamos haciendo Euler, luego haremos Euler semi explicito y luego si da tiempo verlet
	// Calculamos la nueva posicion y velocidad usando euler, para la posicion siguiente += velocidad * variacion de tiempo
	// y para la velocidad siguiente += aceleracion * variacion de tiempo. 
	// Mas adelante haremos que la aceleracion solo cambie con fuerzas, 
	// y como la velocidad solo cambia con la aceleracion y la pos con la velocidad, entonces ya lo tendremos todo.

	float dt = static_cast<float>(t);	// internamente al operar con vectores de PhysX, 
										// es mejor usar float que double ya que physx trabaja con float 
										// y lo va a convertir de todos modos por debajo.

	if (tipoIntegr == TipoIntegracion::Euler) { // Euler
		pos.p += vel.toPxVec3() * dt;
		vel += acc * dt;
		vel *= std::pow(damping, dt);
	}
	else if (tipoIntegr == TipoIntegracion::EulerSemiExplic) { // Euler semi explicito
		vel += acc * t;
		vel *= std::pow(damping, dt);
		pos.p += vel.toPxVec3() * dt;
	}

}