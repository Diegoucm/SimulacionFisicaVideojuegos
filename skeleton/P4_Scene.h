#pragma once

#include "Scene.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "GravityGenerator.h"
#include "SpringForceGenerator.h"
#include "ForceRegistry.h"
#include "AnchoredSpring.h"

class P4_Scene : public Scene {
public:
	explicit P4_Scene(std::string name) : Scene(std::move(name)) {};

	void init() override {
		// 2 particulas unidas por muelle (para un solo muelle que une dos particulas hace falta dos SpringForceGenerator, uno para cada particula)
		Particle* p1 = new Particle(Vector3D(0, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 1.0f);
		Particle* p2 = new Particle(Vector3D(-10, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 1.0f);

		p1->setMass(1.0f);
		p2->setMass(5.0f);

		SpringForceGenerator* spring = new SpringForceGenerator(10.0f, 5.0f, p2);
		
		m_registry.add(p1, spring);

		SpringForceGenerator* spring2 = new SpringForceGenerator(10.0f, 5.0f, p1);

		m_registry.add(p2, spring2);

		m_particles.push_back(p1);
		m_particles.push_back(p2);

		// una sola particula anclada a un punto fijo con muelle
		Particle* p3 = new Particle(Vector3D(20, 10, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1.0f, 1.0f);
		AnchoredSpring* anchoredSpring = new AnchoredSpring(20.0f, 0.0f, Vector3D(20, 20, 0));
		m_registry.add(p3, anchoredSpring);
		m_particles.push_back(p3);

	}

	void update (double dt) override {
		m_registry.update(dt);
		for (Particle* p : m_particles) {
			p->integrate(dt, TipoIntegracion::EulerSemiExplic);
		}
	}

	void cleanup() override {
		for (Particle* p : m_particles) delete p;
		m_particles.clear();
		m_registry.clear();
	}

	void keyPress(unsigned char key, const physx::PxTransform& camera) override {
		if (key == 'g' || key == 'G') {
			if (!m_gravity) {
				m_gravity = new GravityGenerator(Vector3D(0, -9.8f, 0));
				for (Particle* p : m_particles) {
					m_registry.add(p, m_gravity);
				}
			}
			else {
				for (Particle* p : m_particles) {
					m_registry.remove(p, m_gravity);
				}
				delete m_gravity;
				m_gravity = nullptr;
			}
		}
	}

private:
	std::vector<Particle*> m_particles;
	SpringForceGenerator* m_spring = nullptr;
	GravityGenerator* m_gravity = nullptr;


	ForceRegistry m_registry;


};