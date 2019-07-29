#include "Server.h"

#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <vector>
#include <chrono>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameMessages.h"

int nextClientID = 1;
int nextServerID = 1000;

/////////////////////////////////////////////
void Server::Startup()
{
   std::cout << "Starting up the server..." << std::endl;

   //get an instance of the peer interface
   _peerInterface = RakNet::RakPeerInterface::GetInstance();

   //create a descriptor for this socket
   RakNet::SocketDescriptor sd(PORT, 0);

   _peerInterface->Startup(32, &sd, 1);
   _peerInterface->SetMaximumIncomingConnections(32);

   std::cout << "Server started." << std::endl;

   //updates server's game objects
   std::thread updateThread(UpdateThread, this, _peerInterface);

   HandleNetworkMessages();
}

/////////////////////////////////////
// Handles each packet recieved
/////////////////////////////////////
void Server::HandleNetworkMessages()
{
   RakNet::Packet* packet = nullptr;

   while (true)
   {
      for (packet = _peerInterface->Receive(); packet;
          _peerInterface->DeallocatePacket(packet),
           packet =_peerInterface->Receive())
      {
         switch (packet->data[0])
         {
         case ID_NEW_INCOMING_CONNECTION:
            //usernameForAddress[packet->systemAddress] = "ANON";
            std::cout << "A connection is incoming. \n";
            SendNewClientID(packet->systemAddress);
            break;
         case  ID_DISCONNECTION_NOTIFICATION:
            std::cout << "A client has disconnected. \n";
            break;
         case ID_CONNECTION_LOST:
            std::cout << "A client lost connection. \n";
            break;
         case ID_CLIENT_CHANGE_USERNAME:
            ChangeClientName(packet);
         case ID_CLIENT_CHAT_MESSAGE:
            ForwardMessage(packet);
            break;
         case ID_CLIENT_OBJECT_DATA:
         {
            RakNet::BitStream bs(packet->data, packet->length, false);
           _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                       packet->systemAddress, true);

           GameObject clientData;
           clientData.Read(packet);
           _gameObjects[clientData.id] = clientData;
            break;
         }
         case ID_CLIENT_SPAWN_BULLET:
            OnSpawnBullet(packet);
         default:
            std::cout << "Recieved a message with an unknown id: "
               << packet->data[0];
            break;
         }
      }
   }
}

void Server::UpdateThread(Server * server, RakNet::RakPeerInterface * peerInterface)
{
   const int deltaTime = 17;
   while (true)
   {
      std::vector<int> deathRow;
      for (auto& pair : server->_gameObjects)
      {
         GameObject& obj = pair.second;
         if (obj.id >= 1000)
         {
            obj.Update(deltaTime * 0.001f);
            obj.Write(peerInterface, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

            obj.lifetime -= deltaTime * 0.001f;
            if (obj.lifetime <= 0)
               deathRow.push_back(obj.id);
         }
      }
      // despawns the object corresponding to each ID in deathRow
      for (auto& id : deathRow)
      {
         server->Despawn(id);
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(deltaTime));
   }
}

void Server::ForwardMessage(RakNet::Packet * packet)
{
   std::cout << "Forwarding message from client. \n";
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

   std::string str;
   bsIn.Read(str);
   RakNet::BitStream bs;

   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CHAT_MESSAGE);

   bs.Write(_usernameForAddress[packet->systemAddress] + " says: " + str);

   _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Server::ChangeClientName(RakNet::Packet * packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

   std::string ostr = _usernameForAddress[packet->systemAddress];
   RakNet::RakString str;
   bsIn.Read(str);
   std::cout << "Changed Client's username from " << ostr << " to " << str << std::endl;
   _usernameForAddress[packet->systemAddress] = str;
}

void Server::OnSpawnBullet(RakNet::Packet * packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
   glm::vec3 pos;
   glm::vec3 vel;
   bsIn.Read((char*)&pos, sizeof(glm::vec3));
   bsIn.Read((char*)&vel, sizeof(glm::vec3));
   SpawnObject(pos, vel);
   _gameObjects[nextServerID - 1].lifetime = 5.0f;
}

/////////////////////////////////////////////
// pings the clients once per second
/////////////////////////////////////////////
void Server::SendClientPing()
{
   while (true)
   {
      RakNet::BitStream bs;
      bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_TEXT_MESSAGE);
      bs.Write("Ping!");
     _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                          RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
      std::this_thread::sleep_for(std::chrono::seconds(10));
   }
}

void Server::Despawn(int id)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_DESPAWN);
   bs.Write(id);
   _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                          RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

   _gameObjects.erase(id);
}

void Server::SendNewClientID(RakNet::SystemAddress& address)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_SET_CLIENT_ID);
   bs.Write(nextClientID);
   nextClientID++;

  _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED,
                       0, address, false);

  // send all objects to new client
  for (auto it = _gameObjects.begin(); it != _gameObjects.end(); it++)
  {
     GameObject obj = it->second;
     obj.Write(_peerInterface, address, false);
  }

  // send new to all other clients
  int id = nextClientID - 1;
  GameObject obj;
  obj.data.colour = GameObject::GetColour(id);
  obj.id = id;
  obj.Write(_peerInterface, address, true);
}

void Server::SpawnObject(glm::vec3 pos, glm::vec3 vel)
{
   _gameObjects[nextServerID] = GameObject(nextServerID, pos, vel);
   nextServerID++;
}
