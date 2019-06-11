#include "RigidBody.h"
#include <Gizmos.h>

const float MIN_LINEAR_THRESHOLD = 0.1f;

using namespace glm;
using namespace aie;


RigidBody::RigidBody(ShapeType shapeID, 
					 glm::vec2 position, glm::vec2 velocity,
                     float rotation, float mass, float elasticity) :
							PhysicsObj(shapeID), 
							m_position(position), m_velocity(velocity),
							m_rotation(rotation), m_mass(mass), m_elasticity(elasticity)
{
	m_drag = 0.3f;
   m_isKinematic = false;
}


RigidBody::~RigidBody() {}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (m_isKinematic) 
		return;

	applyForce(gravity * m_mass * timeStep);
	m_position += m_velocity * timeStep;

	m_velocity -= m_velocity * m_drag * timeStep;
	if (length(m_velocity) < MIN_LINEAR_THRESHOLD)
		m_velocity = vec2(0, 0);

}

void RigidBody::debug()
{}

void RigidBody::applyForce(glm::vec2 force)
{ m_velocity += force/m_mass; }

void RigidBody::applyForceToActor(RigidBody * actor2, glm::vec2 force)
{
	actor2->applyForce(force);
	this->applyForce(-force);
}

void RigidBody::resolveCollision(RigidBody * actor2)
{
	vec2 normal = normalize(actor2->getPosition() - m_position);
	vec2 relativeVelocity = actor2->getVelocity() - m_velocity;

	float elasticity = 1;
	float j = dot(-(1 + elasticity) * (relativeVelocity), normal) /
		dot(normal, normal * ((1 / m_mass) + (1 / actor2->getMass())));

	vec2 force = normal * j;

	applyForceToActor(actor2, force);
}

vec2 RigidBody::getMomentum()
{
	return m_mass * m_velocity;
}

Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float elasticity, float radius, glm::vec4 colour):
	RigidBody(SPHERE, position, velocity, 0, mass, elasticity),
	m_radius(radius),
	m_colour(colour)
{}



Sphere::~Sphere()
{}

void Sphere::makeGizmo()
{
	Gizmos::add2DCircle(m_position, m_radius, 12, m_colour);
}
