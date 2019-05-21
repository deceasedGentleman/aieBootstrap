#pragma once
#include <glm\ext.hpp>
#include <Gizmos.h>
enum ShapeType
{
	PLANE,
	SPHERE,
	//AABB,
	SHAPE_COUNT
};

class PhysicsObj
{
protected:
	PhysicsObj(ShapeType a_shapeID): m_shapeID(a_shapeID) {}

public:

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep) = 0;
	virtual void debug() = 0;
	virtual void makeGizmo() = 0;
	virtual void resetPosition() {};

	ShapeType getShapeID()const { return m_shapeID; }

protected:
	ShapeType m_shapeID;
};

