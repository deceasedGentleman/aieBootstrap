#pragma once

#include <map>
#include <RakPeerInterface.h>
#include "GameObject.h"

class Server
{
public:
   void Startup();
   void HandleNetworkMessages();
   static void UpdateThread(Server* server, RakNet::RakPeerInterface* peerInterface);

   void SendClientPing();
   void Despawn(int id);
   void SendNewClientID(RakNet::SystemAddress& address);
   void SpawnObject(glm::vec3 position, glm::vec3 velocity);

   void ForwardMessage(RakNet::Packet * packet);
   void ChangeClientName(RakNet::Packet * packet);
   void OnSpawnBullet(RakNet::Packet * packet);

protected:
   std::map<int, GameObject> _gameObjects;
   std::map<RakNet::SystemAddress, std::string> _usernameForAddress;
   RakNet::RakPeerInterface* _peerInterface = nullptr;
   const unsigned int short PORT = 5456;
};

