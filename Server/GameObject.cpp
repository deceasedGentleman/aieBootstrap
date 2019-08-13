#include <Bitstream.h>

#include "GameMessages.h"
#include "GameObject.h"

void GameObject::Update(float deltaTime)
{
   data.position += data.velocity * deltaTime;
}

void GameObject::Write(RakNet::RakPeerInterface * peerInterface, const RakNet::SystemAddress & address, bool broadcast)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_OBJECT_DATA);
   bs.Write(id);
   bs.Write((char*)&data, sizeof(GameObjectData));
   peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED,
                       0, address, broadcast);
}

void GameObject::Read(RakNet::Packet* packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
   bsIn.Read(id);
   bsIn.Read((char*)&data, sizeof(GameObjectData));
}

glm::vec4 colours[] = {
   glm::vec4(0.5,0.5,0.5,1), // 0: grey
   glm::vec4(1,0,0,1),       // 1: red
   glm::vec4(0,0,1,1),       // 2: blue
   glm::vec4(0,1,0,1),       // 3: green
   glm::vec4(1,1,0,1),       // 4: yellow
   glm::vec4(1,0,1,1),       // 5: magenta
   glm::vec4(0,1,1,1),       // 6: cyan
   glm::vec4(0,0,0,1),       // 7: black
};

glm::vec4 GameObject::GetColour(int id)
{
   return colours[id & 7];
}