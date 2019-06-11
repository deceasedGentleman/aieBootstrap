#pragma once
#include "PhysicsObj.h"
#include <glm/ext.hpp>
#include <vector>

class PhysicsScene
{
public:
	PhysicsScene() : PhysicsScene(0.01f, glm::vec2(0,-1)) {}
	PhysicsScene(float timeStep, glm::vec2 gravity) : m_timeStep(timeStep), m_gravity(gravity) {}
	~PhysicsScene();

	void addActor(PhysicsObj* actor);
	void removeActor(PhysicsObj* actor);
	void update(float dt);
	void updateGizmos();

	
	void CheckForCollisions();

	void setGravity(const glm::vec2 gravity) { m_gravity = gravity; }
	glm::vec2 getGravity() const { return m_gravity; }

	void setTimeStep(const float timeStep) { m_timeStep = timeStep; }
	float getTimeStep() const { return m_timeStep; }

	static bool plane2plane(PhysicsObj*, PhysicsObj*) { return false; }
	static bool plane2sphere(PhysicsObj*, PhysicsObj*);
	static bool sphere2plane(PhysicsObj*, PhysicsObj*);
	static bool sphere2sphere(PhysicsObj*, PhysicsObj*);
	
	void debugScene() {};

protected:
	glm::vec2 m_gravity;
	float m_timeStep;
	std::vector<PhysicsObj*> m_actors;
};

