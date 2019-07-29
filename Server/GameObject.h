#pragma once

#include "../dependencies/glm/glm/glm.hpp"
#include <RakPeerInterface.h>

struct GameObjectData
{
   glm::vec3 position;
   glm::vec3 velocity;
   glm::vec4 colour;
};

class GameObject
{
public:
   GameObject(int ID = 0, 
              glm::vec3 pos = glm::vec3(0),
              glm::vec3 vel = glm::vec3(0),
              glm::vec4 colour = glm::vec4(1))
   {
      id = ID;
      data.position = pos;
      data.velocity = vel;
      data.colour = colour;
   }
   GameObject(int ID, GameObjectData dat)
   {
      id = ID;
      data = dat;
   }

   void Update(float deltaTime);

   void Write(RakNet::RakPeerInterface* peerInterface, const RakNet::SystemAddress& address, bool broadcast = 0);
   void Read(RakNet::Packet* packet);

   static glm::vec4 GetColour(int id);

   int id;
   float lifetime;
   GameObjectData data;
};

