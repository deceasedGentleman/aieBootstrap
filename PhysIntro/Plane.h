#pragma once
#include "PhysicsObj.h"
#include "RigidBody.h"
class Plane :
	public PhysicsObj
{
public:
	Plane();
	Plane(glm::vec2 normal, float distance);
	~Plane();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep) {}
	virtual void debug() {}
	virtual void makeGizmo();
	virtual void resetPosition() {}

	void resolveCollision(RigidBody* actor2, const glm::vec2& collisionNormal);

	glm::vec2 getNormal()const { return m_normal; }
	float getDistance()const { return m_distanceToOrigin; }
	
private:
	glm::vec2 m_normal;
	float m_distanceToOrigin;
};

