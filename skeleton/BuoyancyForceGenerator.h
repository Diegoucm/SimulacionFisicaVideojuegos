#pragma once

#include "ForceGenerator.h"

//   E = densidad_liquido * volumen_sumergido * g,   con   Vs = V * immersed
// 'h' = y del CENTRO del objeto, 'h0' = y de la superficie del liquido.
class BuoyancyForceGenerator : public ForceGenerator
{
public:
	BuoyancyForceGenerator(float height, float volume, float liquidDensity, float surfaceY, float gravity = 9.8f);

	void updateForce(Particle* p, double dt) override;

	~BuoyancyForceGenerator() override = default;

	// Fraccion sumergida (0..1) de un cubo de arista 'height' con el centro en y = h,
	// y superficie del liquido en y = h0. La reutiliza el generador de rozamiento.
	static float immersedFraction(float h, float h0, float height);

	// setters: el enunciado pide poder cambiar volumen (y densidad del liquido) por teclado
	void setHeight(float h) { _height = h; }
	void setVolume(float v) { _volume = v; }
	void setLiquidDensity(float d) { _liquidDensity = d; }
	void setSurfaceY(float y) { _surfaceY = y; }

	// getters: utiles para imprimir datos de depuracion
	float getHeight() const { return _height; }
	float getVolume() const { return _volume; }
	float getLiquidDensity() const { return _liquidDensity; }
	float getSurfaceY() const { return _surfaceY; }

protected:
	float _height;          // arista del cubo
	float _volume;
	float _liquidDensity;   // kg/m^3 (agua = 1000)
	float _surfaceY;        // y de la superficie del liquido
	float _gravity;         // mismo valor que el GravityGenerator (9.8)
};