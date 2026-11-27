#pragma once

#include "Scene.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "GravityGenerator.h"
#include "SpringForceGenerator.h"
#include "ForceRegistry.h"
#include "AnchoredSpring.h"

#include <iostream>
#include <cmath>

// Teclas: g gravedad | f fuerza breve | k / j sube / baja k del muelle anclado | e integrador
//         b amortiguamiento: ninguno -> subamortiguado -> critico -> sobreamortiguado
//         p imprime datos cada 0.5 s para comprobar (y del anclado vs equilibrio, distancia del par)
class P4_Scene : public Scene {
public:
	explicit P4_Scene(std::string name) : Scene(std::move(name)) {};

	void init() override {
		// OJO: damping de particula = 1.0 (sin el truco del integrador) para que el unico amortiguamiento sea el del muelle.
		// 2 particulas unidas por muelle (para un solo muelle que une dos particulas hace falta dos SpringForceGenerator, uno para cada particula)
		m_p1 = new Particle(Vector3D(10, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 1.0f);
		m_p2 = new Particle(Vector3D(-10, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 1.0f);

		m_p1->setMass(1.0f);
		m_p2->setMass(5.0f);

		m_pairSpring1 = new SpringForceGenerator(10.0f, 12.0f, m_p2);
		m_generators.push_back(m_pairSpring1);
		m_registry.add(m_p1, m_pairSpring1);

		m_pairSpring2 = new SpringForceGenerator(10.0f, 12.0f, m_p1);
		m_generators.push_back(m_pairSpring2);
		m_registry.add(m_p2, m_pairSpring2);

		m_particles.push_back(m_p1);
		m_particles.push_back(m_p2);

		// una sola particula anclada a un punto fijo con muelle
		m_p3 = new Particle(Vector3D(20, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 1.0f);
		m_anchoredSpring = new AnchoredSpring(m_k, m_rest, Vector3D(20, m_anchorY, 0));
		m_generators.push_back(m_anchoredSpring);
		m_registry.add(m_p3, m_anchoredSpring);
		m_particles.push_back(m_p3);

		m_gravity = new GravityGenerator(Vector3D(0, -9.8f, 0));
		m_gravity->setActive(false);                 // empieza desactivada
		m_generators.push_back(m_gravity);
		for (Particle* p : m_particles)
			m_registry.add(p, m_gravity);

		applyDamping();
	}

	void update(double dt) override {
		m_registry.update(dt);

		if (m_impulseTime > 0.0) {
			m_p3->addForce(Vector3D(50, 0, 0));   // ajusta magnitud/direccion
			m_impulseTime -= dt;
		}

		for (Particle* p : m_particles)
			p->integrate(dt, m_integ);

		if (m_debug) {
			m_dbgTimer += dt;
			if (m_dbgTimer >= 0.5) { m_dbgTimer = 0.0; printDebug(); }
		}
	}

	void cleanup() override {
		m_registry.clear();                          // primero el registro, para que no queden punteros usados
		for (Particle* p : m_particles) delete p;
		m_particles.clear();

		for (ForceGenerator* g : m_generators) delete g;   // el destructor de AnchoredSpring borra su anclaje
		m_generators.clear();

		m_gravity = nullptr;
		m_anchoredSpring = nullptr;
		m_pairSpring1 = m_pairSpring2 = nullptr;
		m_p1 = m_p2 = m_p3 = nullptr;
	}

	void keyPress(unsigned char key, const physx::PxTransform& camera) override {
		if (key == 'g' || key == 'G') {
			m_gravity->toggle();
			std::cout << "Gravedad " << (m_gravity->isActive() ? "ON" : "OFF") << std::endl;
		}

		if (key == 'f' || key == 'F') m_impulseTime = 0.2;          // fuerza breve

		if (key == 'k' || key == 'K') {                              // subir k
			m_k *= 1.5;
			m_anchoredSpring->setK(m_k);
			applyDamping();                                          // bc depende de k
			std::cout << "k = " << m_k << std::endl;
		}
		if (key == 'j' || key == 'J') {                              // bajar k
			m_k /= 1.5;
			m_anchoredSpring->setK(m_k);
			applyDamping();
			std::cout << "k = " << m_k << std::endl;
		}

		if (key == 'e' || key == 'E') {                              // alternar integrador
			m_integ = (m_integ == TipoIntegracion::Euler)
				? TipoIntegracion::EulerSemiExplic : TipoIntegracion::Euler;
			std::cout << (m_integ == TipoIntegracion::Euler ? "Euler" : "Euler semi-explicito") << std::endl;
		}

		if (key == 'b' || key == 'B') {                              // ciclar amortiguamiento
			m_dampIdx = (m_dampIdx + 1) % 4;
			applyDamping();
			std::cout << "Amortiguamiento: " << DAMP_NAMES[m_dampIdx]
				<< " (b = " << DAMP_FACTORS[m_dampIdx] << " * bc)"
				<< " | anclado: bc = " << bcAnchored() << ", b = " << m_anchoredSpring->getDamping()
				<< std::endl;
		}

		if (key == 'p' || key == 'P') { m_debug = !m_debug; m_dbgTimer = 0.5; }
	}

private:
	// bc = 2 * sqrt(k * m)
	// - anclado: m = masa de la particula
	// - par de particulas: m = masa reducida mu = m1*m2/(m1+m2) (es la masa "efectiva" de la oscilacion relativa)
	double bcAnchored() const { return 2.0 * std::sqrt(m_k * m_p3->getMass()); }

	double bcPair() const {
		const double m1 = m_p1->getMass(), m2 = m_p2->getMass();
		const double mu = m1 * m2 / (m1 + m2);
		return 2.0 * std::sqrt(m_pairSpring1->getK() * mu);
	}

	void applyDamping() {
		const double z = DAMP_FACTORS[m_dampIdx];     // b / bc
		m_anchoredSpring->setDamping(z * bcAnchored());
		m_pairSpring1->setDamping(z * bcPair());      // los dos generadores del par con el mismo b
		m_pairSpring2->setDamping(z * bcPair());
	}

	void printDebug() {
		const float y = m_p3->getPosition().p.y;
		// equilibrio teorico del anclado (muelle vertical): anclaje - reposo - (m g / k si hay gravedad)
		double yEq = m_anchorY - m_rest;
		if (m_gravity->isActive()) yEq -= m_p3->getMass() * 9.8 / m_k;
		const float dPair = (m_p1->getPosition().p - m_p2->getPosition().p).magnitude();
		std::cout << "[muelles] anclado y=" << y << " (equilibrio " << yEq << ")"
			<< " | par distancia=" << dPair << " (reposo 12)" << std::endl;
	}

	static constexpr int N_MODES = 4;
	const double DAMP_FACTORS[N_MODES] = { 0.0, 0.25, 1.0, 3.0 };           // b / bc
	const char* DAMP_NAMES[N_MODES] = { "sin amortiguar", "subamortiguado", "critico", "sobreamortiguado" };

	std::vector<Particle*> m_particles;
	std::vector<ForceGenerator*> m_generators;   // dueno de todos los generadores

	GravityGenerator* m_gravity = nullptr;

	ForceRegistry m_registry;

	Particle* m_p1 = nullptr;
	Particle* m_p2 = nullptr;
	Particle* m_p3 = nullptr;
	SpringForceGenerator* m_pairSpring1 = nullptr;   // fuerza sobre p1
	SpringForceGenerator* m_pairSpring2 = nullptr;   // fuerza sobre p2
	AnchoredSpring* m_anchoredSpring = nullptr;

	double m_k = 20.0;
	double m_rest = 6.7;
	float  m_anchorY = 20.f;
	double m_impulseTime = 0.0;                 // segundos que le quedan a la fuerza
	int    m_dampIdx = 0;
	bool   m_debug = false;
	double m_dbgTimer = 0.0;
	TipoIntegracion m_integ = TipoIntegracion::EulerSemiExplic;
};