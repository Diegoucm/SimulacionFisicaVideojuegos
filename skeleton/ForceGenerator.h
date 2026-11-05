#pragma once
class Particle;

class ForceGenerator {
public:
    virtual ~ForceGenerator() = default;
    virtual void updateForce(Particle* p, double dt) = 0;

	bool isActive() const { return m_enabled; }
	void setActive(bool a) { m_enabled = a; }
	void toggle() { m_enabled = !m_enabled; }

private:
	bool m_enabled = true;
};