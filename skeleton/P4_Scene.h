#pragma once

#include "Scene.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "GravityGenerator.h"
#include "SpringForceGenerator.h"
#include "ForceRegistry.h"
#include "AnchoredSpring.h"

#include <iostream>

class P4_Scene : public Scene {
public:
	explicit P4_Scene(std::string name) : Scene(std::move(name)) {};

	void init() override {
		// 2 particulas unidas por muelle (para un solo muelle que une dos particulas hace falta dos SpringForceGenerator, uno para cada particula)
		Particle* p1 = new Particle(Vector3D(10, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 0.98f, 1.0f);
		Particle* p2 = new Particle(Vector3D(-10, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 0.98f, 1.0f);

		p1->setMass(1.0f);
		p2->setMass(5.0f);

		SpringForceGenerator* spring = new SpringForceGenerator(10.0f, 12.0f, p2);
		
		m_generators.push_back(spring);
		m_registry.add(p1, spring);

		SpringForceGenerator* spring2 = new SpringForceGenerator(10.0f, 12.0f, p1);

		m_generators.push_back(spring2);
		m_registry.add(p2, spring2);

		m_particles.push_back(p1);
		m_particles.push_back(p2);

		// una sola particula anclada a un punto fijo con muelle
		m_p3 = new Particle(Vector3D(20, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 1.0f);
		m_anchoredSpring = new AnchoredSpring(m_k, 6.7f, Vector3D(20, 20, 0));
		m_generators.push_back(m_anchoredSpring);
		m_registry.add(m_p3, m_anchoredSpring);
		m_particles.push_back(m_p3);


		m_gravity = new GravityGenerator(Vector3D(0, -9.8f, 0));
		m_gravity->setActive(false);                 // empieza desactivada
		m_generators.push_back(m_gravity);
		for (Particle* p : m_particles)
			m_registry.add(p, m_gravity);

	}

	void update (double dt) override {
		m_registry.update(dt);

		if (m_impulseTime > 0.0) {
			m_p3->addForce(Vector3D(50, 0, 0));   // ajusta magnitud/dirección
			m_impulseTime -= dt;
		}

		for (Particle* p : m_particles)
			p->integrate(dt, m_integ);
	}

	void cleanup() override {
		m_registry.clear();                          // primero el registro, para que no queden punteros usados
		for (Particle* p : m_particles) delete p;
		m_particles.clear();

		for (ForceGenerator* g : m_generators) delete g;   // el destructor de AnchoredSpring borra su anclaje
		m_generators.clear();

		m_gravity = nullptr;
		m_anchoredSpring = nullptr;
		m_p3 = nullptr;
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
			std::cout << "k = " << m_k << std::endl;
		}
		if (key == 'j' || key == 'J') {                              // bajar k
			m_k /= 1.5;
			m_anchoredSpring->setK(m_k);
			std::cout << "k = " << m_k << std::endl;
		}

		if (key == 'e' || key == 'E') {                              // alternar integrador
			m_integ = (m_integ == TipoIntegracion::Euler)
				? TipoIntegracion::EulerSemiExplic : TipoIntegracion::Euler;
			std::cout << (m_integ == TipoIntegracion::Euler ? "Euler" : "Euler semi-explicito") << std::endl;
		}
	}

private:

	std::vector<Particle*> m_particles;
	std::vector<ForceGenerator*> m_generators;   // dueño de todos los generadores

	GravityGenerator* m_gravity = nullptr;

	ForceRegistry m_registry;

	Particle* m_p3 = nullptr;
	AnchoredSpring* m_anchoredSpring = nullptr;
	double m_k = 20.0;
	double m_impulseTime = 0.0;                 // segundos que le quedan a la fuerza
	TipoIntegracion m_integ = TipoIntegracion::EulerSemiExplic;
};