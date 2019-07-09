#include <Bitstream.h>

#include "GameMessages.h"
#include "GameObject.h"

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
