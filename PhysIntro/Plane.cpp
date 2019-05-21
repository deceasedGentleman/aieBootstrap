#include "Plane.h"

using namespace glm;


Plane::Plane(): Plane(glm::vec2(0,0),0)
{}

Plane::Plane(glm::vec2 normal, float distance):
	PhysicsObj(PLANE),
	m_normal(glm::normalize(normal)),
	m_distanceToOrigin(distance)
{}


Plane::~Plane()
{}

void Plane::makeGizmo()
{
	float lineSegmentLength = 300;
	vec2 centerPoint = m_normal * m_distanceToOrigin;
	vec2 parallel(m_normal.y, -m_normal.x);
	vec4 colour(1, 1, 1, 1);
	vec2 start = centerPoint + (parallel * lineSegmentLength);
	vec2 end = centerPoint - (parallel * lineSegmentLength);
	aie::Gizmos::add2DLine(start, end, colour);
}

void Plane::resolveCollision(RigidBody* actor2, const glm::vec2& nCol)
{
	float elasticity = 1;
	vec2 vel = actor2->getVelocity();
	vel = vel - (1 + elasticity)* glm::dot(vel, nCol) * nCol;
	actor2->setVelocity(vel);
}
