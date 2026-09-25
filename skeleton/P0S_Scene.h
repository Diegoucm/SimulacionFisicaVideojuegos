#pragma once

#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>


class P0S_Scene : public Scene {
public:
	explicit P0S_Scene(std::string name) : Scene(std::move(name)) {};
	void init () override {
		// Inicialización de la escena
		physx::PxShape* shape = CreateShape(physx::PxSphereGeometry(2.0f));
		
		m_transform = physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f));
		m_renderItem = new RenderItem(shape, &m_transform, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

    void update(double dt) override {
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
};