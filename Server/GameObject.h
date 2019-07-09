#pragma once

#include "../dependencies/glm/glm/glm.hpp"
#include <RakPeerInterface.h>

struct GameObjectData
{
   glm::vec3 position;
   glm::vec4 colour;
};

class GameObject
{
public:
   GameObject()
   {
      data.position = glm::vec3(0);
      data.colour = glm::vec4(1, 1, 1, 1);
   }
   ~GameObject();

   void Write(RakNet::RakPeerInterface* peerInterface, const RakNet::SystemAddress& address, bool broadcast = 0);
   void Read(RakNet::Packet* packet);

   int id;
   GameObjectData data;
};

