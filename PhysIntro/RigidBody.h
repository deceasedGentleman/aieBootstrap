#pragma once
#include "PhysicsObj.h"
class RigidBody : public PhysicsObj
{
public:
	RigidBody(ShapeType shapeID, glm::vec2 position,
				 glm::vec2 velocity, float rotation, float mass, float elasticity);
	~RigidBody();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug();
	void applyForce(glm::vec2 force);
	void applyForceToActor(RigidBody* actor2, glm::vec2 force);

	void resolveCollision(RigidBody* actor2);
	
	glm::vec2 getMomentum();

	glm::vec2 getPosition() { return m_position; }
	glm::vec2 getVelocity() { return m_velocity; }
	float getMass() { return (m_isKinematic) ? INT_MAX : m_mass; }
	float getRotation() { return m_rotation; }
	float getDrag() { return m_drag; }
	float getElasticity() { return m_elasticity; }
	bool  isKinematic() { return m_isKinematic; }

	void setPosition(glm::vec2 pos) { m_position = pos; }
	void setVelocity(glm::vec2 vel) { m_velocity = vel; }
	void setMass(float mas)		  { m_mass     = mas; }
	void setRotation(float rot)	  { m_rotation = rot; }
	void setDrag(float drg) { m_drag = drg; }
	void setElasticity(float ela) { m_elasticity = ela; }
	void setKinematic(bool state) { m_isKinematic = state; } 


protected:
	glm::vec2 m_position;
	glm::vec2 m_velocity;
	float m_mass;
	float m_rotation; 
	float m_drag;
	float m_elasticity;
	bool m_isKinematic;
};

class Sphere : public RigidBody
{
public:
	Sphere(glm::vec2 position, glm::vec2 velocity, 
			 float mass, float elasticity, float radius, glm::vec4 colour);
	Sphere(glm::vec2 position, float inclination, float speed,float mass, float elasticity, float radius, glm::vec4 colour) :
		Sphere(position, glm::vec2(speed * cosf(inclination), speed * sinf(inclination)),
				 mass, elasticity, radius, colour)
	{}
	~Sphere();

	virtual void makeGizmo();
	//virtual bool checkCollision(PhysicsObj* pOther);

	float getRadius() { return m_radius; }
	glm::vec4 getColour() { return m_colour; }

protected:
	float m_radius;
	glm::vec4 m_colour;
};