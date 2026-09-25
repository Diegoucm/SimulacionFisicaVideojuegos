#pragma once

#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>
#include "Particle.h"



class P1_Scene : public Scene {
public:
    explicit P1_Scene(std::string name) : Scene(std::move(name)) {};
    void init() override {
        // Inicialización de la escena
        //physx::PxShape* shape = CreateShape(physx::PxSphereGeometry(2.0f));

        //m_transform = physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f));
        //m_renderItem = new RenderItem(shape, &m_transform, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

		// Crear partículas y agregarlas al vector
		Particle* particle1 = new Particle(Vector3(0.0f, 50.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -9.81f, 0.0f), 1.0f);
		m_particles.push_back(particle1);

		Particle* particle2 = new Particle(Vector3(10.0f, 50.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -9.81f, 0.0f), 0.98f);
		m_particles.push_back(particle2);

		Particle* particle3 = new Particle(Vector3(-10.0f, 50.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -9.81f, 0.0f), 0.0f);
		m_particles.push_back(particle3);
    }

    void update(double dt) override {
        for (Particle* particle : m_particles) {

			// si no se ha salido de unos limites de la pantalla, entonces integramos, si no, la borramos y liberamos memoria
            /*
            if(particle->getPosition().p.y < -100.0f || particle->getPosition().p.y > 100.0f) {
                particle->~Particle(); // Llamamos al destructor para liberar recursos
                delete particle; // Liberamos memoria
                particle = nullptr; // Evitamos dangling pointer
                continue; // Pasamos a la siguiente partícula
            }
            */

            particle->integrate(dt, TipoIntegracion::EulerSemiExplic); // Usando Euler como ejemplo
		}
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        if (key == 'r' || key == 'R') {
            m_transform.p = physx::PxVec3(0.0f, 10.0f, 0.0f); // Reset
        }
    }

    void cleanup() override {
        if (m_renderItem) {
            m_renderItem->release(); // Deregistra y destruye el item
            m_renderItem = nullptr;
        }
    }

private:
    physx::PxTransform m_transform;
    RenderItem* m_renderItem{ nullptr };

	// Vector para almacenar las partículas
	std::vector<Particle*> m_particles;

};