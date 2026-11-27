#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include "Scene.h"
#include "Particle.h"
#include "ForceRegistry.h"
#include "GravityGenerator.h"
#include "BuoyancyForceGenerator.h"
#include "LiquidDragForceGenerator.h"

// Actividad 3: flotacion.
//   - Plano azul muy fino = superficie del agua (y = m_surfaceY).
//   - Cubo naranja (Particle con forma Box) = objeto flotante.
//
// Teclas:
//   m / n : sube / baja la masa (x1.25)
//   v / c : sube / baja el volumen (x1.25) -> el cubo se reconstruye con la nueva arista
//   z     : caso limite (masa = densidad_liquido * volumen, Fg = E)
//   l     : cambia de liquido (agua, aceite, mercurio); cada uno trae su k1 de rozamiento
//   x     : rozamiento con el liquido on/off
//   u / y : sube / baja k1 (viscosidad) x1.5
//   r     : reinicia el objeto (arriba, v = 0)
//   p     : imprime/oculta datos de comprobacion cada segundo
class P4_Buoyancy_Scene : public Scene {
public:
    explicit P4_Buoyancy_Scene(std::string name) : Scene(std::move(name)) {}

    void init() override {
        // ---- superficie del agua: cubo con Y muy pequena ----
        m_waterTransform = physx::PxTransform(physx::PxVec3(0.f, m_surfaceY - 0.05f, 0.f));  // cara superior en m_surfaceY
        m_waterItem = new RenderItem(CreateShape(physx::PxBoxGeometry(15.f, 0.05f, 15.f)),
            &m_waterTransform, Vector4(0.f, 0.3f, 1.f, 1.f));

        // ---- generadores (propiedad de la escena) ----
        m_gravity = new GravityGenerator(Vector3D(0, -9.8f, 0));
        m_buoyancy = new BuoyancyForceGenerator(sideFromVolume(m_volume), m_volume,
            liquid().density, m_surfaceY);

        m_k1 = liquid().k1;
        m_drag = new LiquidDragForceGenerator(m_k1, 0.f, sideFromVolume(m_volume), m_surfaceY);   // k2 = 0 en liquidos

        spawnObject(Vector3D(0.f, m_surfaceY + 8.f, 0.f), Vector3D(0, 0, 0));

        std::cout << "[Flotacion] m/n: masa | v/c: volumen | z: caso limite | l: liquido | x: rozamiento | u/y: k1 | r: reset | p: debug\n";
        printState();
    }

    void update(double dt) override {
        m_registry.update(dt);                                   // 1) acumular fuerzas
        if (m_obj) m_obj->integrate(dt, TipoIntegracion::EulerSemiExplic);   // 2) integrar

        if (m_debug) {
            m_dbgTimer += dt;
            if (m_dbgTimer >= 1.0) { m_dbgTimer = 0.0; printState(); }
        }
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        switch (key) {
        case 'm': case 'M':
            m_mass *= 1.25f; m_obj->setMass(m_mass); printState(); break;
        case 'n': case 'N':
            m_mass /= 1.25f; if (m_mass < 1.f) m_mass = 1.f; m_obj->setMass(m_mass); printState(); break;
        case 'v': case 'V':
            m_volume *= 1.25f; changeVolume(); break;
        case 'c': case 'C':
            m_volume /= 1.25f; if (m_volume < 0.1f) m_volume = 0.1f; changeVolume(); break;
        case 'z': case 'Z':
            m_mass = liquid().density * m_volume;   // Fg = E cuando esta todo sumergido
            m_obj->setMass(m_mass); printState(); break;
        case 'l': case 'L':
            m_liquidIdx = (m_liquidIdx + 1) % 3;
            m_buoyancy->setLiquidDensity(liquid().density);
            m_k1 = liquid().k1; m_drag->setK1(m_k1);
            printState(); break;
        case 'x': case 'X':
            m_drag->toggle(); printState(); break;
        case 'u': case 'U':
            m_k1 *= 1.5f; m_drag->setK1(m_k1); printState(); break;
        case 'y': case 'Y':
            m_k1 /= 1.5f; m_drag->setK1(m_k1); printState(); break;
        case 'r': case 'R':
            spawnObject(Vector3D(0.f, m_surfaceY + 8.f, 0.f), Vector3D(0, 0, 0)); printState(); break;
        case 'p': case 'P':
            m_debug = !m_debug; m_dbgTimer = 1.0; break;
        }
    }

    void cleanup() override {
        m_registry.clear();
        delete m_obj;       m_obj = nullptr;
        delete m_gravity;   m_gravity = nullptr;
        delete m_buoyancy;  m_buoyancy = nullptr;
        delete m_drag;      m_drag = nullptr;
        if (m_waterItem) { m_waterItem->release(); m_waterItem = nullptr; }
    }

private:
    struct Liquid { const char* name; float density; float k1; };   // k1: rozamiento viscoso (N*s/m)
    const Liquid& liquid() const {
        static const Liquid L[3] = { {"agua", 1000.f, 10000.f}, {"aceite", 800.f, 25000.f}, {"mercurio", 13600.f, 15000.f} };
        return L[m_liquidIdx];
    }

    static float sideFromVolume(float v) { return std::cbrt(v); }

    // En tu Particle, Box usa PxBoxGeometry(radius, radius, radius): radius = semi-arista (arista = 2 * radius).
    static float visualRadius(float side) { return side * 0.5f; }

    // (Re)crea el cubo con la arista actual y lo registra con gravedad y empuje.
    void spawnObject(const Vector3D& pos, const Vector3D& vel) {
        if (m_obj) {
            m_registry.removeParticle(m_obj);      // SIEMPRE antes del delete
            delete m_obj;
            m_obj = nullptr;
        }
        const float side = sideFromVolume(m_volume);
        // damping 1.0: sin el truco del integrador; la unica perdida de energia es el rozamiento fisico
        m_obj = new Particle(pos, vel, Vector3D(0, 0, 0), 1.0f, visualRadius(side),
            Vector4(0.9f, 0.55f, 0.15f, 1.f), ParticleShape::Box);
        m_obj->setMass(m_mass);
        m_buoyancy->setVolume(m_volume);
        m_buoyancy->setHeight(side);
        m_drag->setHeight(side);
        m_registry.add(m_obj, m_gravity);
        m_registry.add(m_obj, m_buoyancy);
        m_registry.add(m_obj, m_drag);
    }

    void changeVolume() {
        // conserva posicion y velocidad actuales; solo cambia el tamano
        const physx::PxVec3 p = m_obj->getPosition().p;
        spawnObject(Vector3D(p.x, p.y, p.z), m_obj->getVel());
        printState();
    }

    void printState() {
        const float rho_l = liquid().density;
        const float rho_o = m_mass / m_volume;
        const float side = sideFromVolume(m_volume);
        const float y = m_obj->getPosition().p.y;

        std::string regime;
        if (std::fabs(rho_o - rho_l) < 1e-3f * rho_l) regime = "LIMITE (Fg = E)";
        else if (rho_o < rho_l)                       regime = "FLOTA";
        else                                          regime = "SE HUNDE";

        std::cout << "[Flotacion] liquido=" << liquid().name << " (" << rho_l << ", k1=" << m_k1 << (m_drag->isActive() ? "" : " OFF") << ")"
            << " | m=" << m_mass << " kg, V=" << m_volume << " m3, lado=" << side
            << " | dens.obj=" << rho_o << " -> " << regime;
        if (rho_o < rho_l) {
            const float frac = rho_o / rho_l;                         // fraccion sumergida en equilibrio
            const float yEq = m_surfaceY + side * (0.5f - frac);       // y teorica del centro
            std::cout << " | sumergido=" << frac * 100.f << "% yEq=" << yEq;
        }
        std::cout << " | y actual=" << y << std::endl;
    }

    ForceRegistry m_registry;
    Particle* m_obj = nullptr;
    GravityGenerator* m_gravity = nullptr;
    BuoyancyForceGenerator* m_buoyancy = nullptr;
    LiquidDragForceGenerator* m_drag = nullptr;
    float m_k1 = 10000.f;      // rozamiento viscoso actual

    physx::PxTransform m_waterTransform;
    RenderItem* m_waterItem = nullptr;

    float m_surfaceY = 10.f;   // y de la superficie del agua
    float m_mass = 4000.f;     // kg
    float m_volume = 8.f;      // m3 (arista 2)  -> densidad 500: flota con el 50% sumergido
    int   m_liquidIdx = 0;
    bool  m_debug = false;
    double m_dbgTimer = 0.0;
};