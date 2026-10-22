#pragma once
#include <random>
#include <vector>
#include "Particle.h"

enum class VelDistribution { Uniform, Normal };

// Parametros que definen un emisor. Cambiando solo esto se consiguen efectos distintos.
struct EmitterConfig {
    // --- posicion de nacimiento ---
    Vector3D position = Vector3D(0, 0, 0);
    Vector3D posSpread = Vector3D(0, 0, 0);   // semi-extension de la caja (uniforme). 0 = punto fijo

    // --- velocidad inicial ---
    VelDistribution dist = VelDistribution::Normal;
    Vector3D velMean = Vector3D(0, 10, 0);
    Vector3D velSpread = Vector3D(1, 1, 1);   // Uniform: semi-rango | Normal: desviacion tipica

    // --- fisica y aspecto de cada particula ---
    Vector3D acc = Vector3D(0, -10.0f, 0);    // gravedad del enunciado por defecto
    float damping = 0.99f;
    float radius = 0.3f;
    Vector4 color = Vector4(1, 1, 1, 1);
    float lifeTime = 5.0f;                    // segundos

    // --- ritmo ---
    float rate = 60.0f;                       // particulas por segundo
};

class ParticleGenerator {
public:
    explicit ParticleGenerator(const EmitterConfig& c)
        : cfg(c), rng(std::random_device{}()) {}

    void setEnabled(bool e) { enabled = e; }
    void toggle() { enabled = !enabled; }
    bool isEnabled() const { return enabled; }

    // Crea las particulas que toquen en este frame y las anade a 'out'.
    // Se usa un acumulador para que el ritmo no dependa del framerate.
    void emit(double dt, std::vector<Particle*>& out) {
        if (!enabled) return;
        accum += cfg.rate * dt;
        while (accum >= 1.0) {
            out.push_back(spawn());
            accum -= 1.0;
        }
    }

private:
    Particle* spawn() {
        Vector3D p(uniform(cfg.position.x, cfg.posSpread.x),
                   uniform(cfg.position.y, cfg.posSpread.y),
                   uniform(cfg.position.z, cfg.posSpread.z));

        Vector3D v(sampleVel(cfg.velMean.x, cfg.velSpread.x),
                   sampleVel(cfg.velMean.y, cfg.velSpread.y),
                   sampleVel(cfg.velMean.z, cfg.velSpread.z));

        Particle* part = new Particle(p, v, cfg.acc, cfg.damping, cfg.radius, cfg.color);
        part->setLifeTime(cfg.lifeTime);
        return part;
    }

    // uniforme en [mean-half, mean+half]
    float uniform(float mean, float half) {
        if (half <= 0.0f) return mean;
        std::uniform_real_distribution<float> d(mean - half, mean + half);
        return d(rng);
    }

    float sampleVel(float mean, float spread) {
        if (spread <= 0.0f) return mean;   // normal_distribution exige sigma > 0
        if (cfg.dist == VelDistribution::Uniform) {
            std::uniform_real_distribution<float> d(mean - spread, mean + spread);
            return d(rng);
        }
        std::normal_distribution<float> d(mean, spread);
        return d(rng);
    }

    EmitterConfig cfg;
    std::mt19937 rng;
    double accum = 0.0;
    bool enabled = true;
};
