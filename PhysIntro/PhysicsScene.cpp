#include "PhysicsScene.h"
#include <list>
#include "RigidBody.h"
#include "Plane.h"

using std::find;
using namespace aie;

std::vector<glm::vec2> debug;

PhysicsScene::~PhysicsScene()
{
	for (auto& pActor : m_actors)
	{
		delete pActor;
	}
}

void PhysicsScene::addActor(PhysicsObj * actor)
{ m_actors.push_back(actor); }

void PhysicsScene::removeActor(PhysicsObj * actor)
{
	auto iter = std::find(m_actors.begin(), m_actors.end(), actor);

	if (iter != m_actors.end())
	{
		m_actors.erase(iter);
	}
}

void PhysicsScene::update(float dt)
{

	static float accTime = 0.f;
	accTime += dt;

	while (accTime >= m_timeStep)
	{
		for (auto pActor : m_actors)
		{
			pActor->fixedUpdate(m_gravity, m_timeStep);
		}
		CheckForCollisions();
		accTime -= m_timeStep;
	}
}

void PhysicsScene::updateGizmos()
{
	for (auto pActor : m_actors)
	{
		pActor->makeGizmo();
	}
	for (auto collision : debug)
	{
		Gizmos::add2DCircle(collision, 1.f, 12, glm::vec4(1, 0, 1, 1));
	}
	debug.clear();
}

//////////////////////////////////////////////////////
// Collisions
//////////////////////////////////////////////////////

//function point for collisions
typedef bool(*fn)(PhysicsObj*, PhysicsObj*);

static fn collisionFunctionArray[] =
{
	PhysicsScene::plane2plane, PhysicsScene::plane2sphere,
	PhysicsScene::sphere2plane, PhysicsScene::sphere2sphere
};

void PhysicsScene::CheckForCollisions()
{
	size_t actorCount = m_actors.size();
	for (size_t outer = 0; outer < actorCount -1; outer++)
	{
		for (size_t inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsObj* object_A = m_actors[outer];
			PhysicsObj* object_B = m_actors[inner];
			int shapeID_A = object_A->getShapeID();
			int shapeID_B = object_B->getShapeID();

			int functionIndex = (shapeID_A * SHAPE_COUNT) + shapeID_B;
			fn collisionFunctionPtr = collisionFunctionArray[functionIndex];
			if (collisionFunctionPtr)
				collisionFunctionPtr(object_A, object_B);
		}
	}
}

bool PhysicsScene::plane2sphere(PhysicsObj * obj1, PhysicsObj * obj2)
{ return sphere2plane(obj2, obj1); }

bool PhysicsScene::sphere2plane(PhysicsObj *obj1, PhysicsObj *obj2)
{ 
	Sphere *sphere = dynamic_cast<Sphere*>(obj1);
	Plane *plane = dynamic_cast<Plane*>(obj2);
	if (sphere && plane)
	{
		float totalMomentumBefore = glm::length(sphere->getMomentum());
		glm::vec2 colNormal = plane->getNormal();
		float dist = glm::dot(sphere->getPosition(),
									 plane->getNormal()) - plane->getDistance();
		if ( dist < 0)
		{
			dist *= -1;
			colNormal *= -1;
		}

		//if (dist <= sphere->getRadius() + plane->getDistance())
		float intersect = sphere->getRadius() - dist;
		if(intersect > 0)
		{
			glm::vec2 collision = sphere->getPosition() + colNormal * -sphere->getRadius();
			debug.push_back(collision);
			plane->resolveCollision(sphere, colNormal);
			float totalMomentumAfter = glm::length(sphere->getMomentum());
			//assert(totalMomentumAfter == totalMomentumBefore /*( * frictionCoeficient)*/);
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphere2sphere(PhysicsObj * obj1, PhysicsObj * obj2)
{
	Sphere *sphere1 = dynamic_cast<Sphere*>(obj1);
	Sphere *sphere2 = dynamic_cast<Sphere*>(obj2);
	float totalMomentumBefore = glm::length(sphere1->getMomentum()) + glm::length(sphere2->getMomentum());
	if (sphere1 && sphere2)
	{
		glm::vec2 colNormal = glm::normalize(sphere1->getPosition() - sphere2->getPosition());
		glm::vec2 collision = sphere1->getPosition() + colNormal * -sphere1->getRadius();

		float dist = glm::distance(sphere1->getPosition(), sphere2->getPosition());
      float pen = -(dist - sphere1->getRadius() + sphere2->getRadius());
		if (pen >= 0)
		{
			debug.push_back(collision);
			sphere1->resolveCollision(sphere2);

         sphere1->setPosition(sphere1->getPosition() + colNormal * pen / 2.f);
         sphere2->setPosition(sphere2->getPosition() - colNormal * pen / 2.f);
			float totalMomentumAfter = glm::length(sphere1->getMomentum()) + glm::length(sphere2->getMomentum());
			return true;
		}
	}
	return false;
}
