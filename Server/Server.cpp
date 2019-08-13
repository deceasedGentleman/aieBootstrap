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
   m_peerInterface = RakNet::RakPeerInterface::GetInstance();

   //create a descriptor for this socket
   RakNet::SocketDescriptor sd(PORT, 0);

   m_peerInterface->Startup(32, &sd, 1);
   m_peerInterface->SetMaximumIncomingConnections(32);

   std::cout << "Server started." << std::endl;

   //updates server's game objects
   std::thread updateThread(UpdateThread, this, m_peerInterface);

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
      for (packet = m_peerInterface->Receive(); packet;
          m_peerInterface->DeallocatePacket(packet),
           packet =m_peerInterface->Receive())
      {
         switch (packet->data[0])
         {
         case ID_NEW_INCOMING_CONNECTION:
            m_usernameForAddress[packet->systemAddress] = "ANON";
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
            m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                 packet->systemAddress, true);

            GameObject clientData;
            clientData.Read(packet);
            m_gameObjects[clientData.id] = clientData;
            break;
         }
         case ID_CLIENT_SPAWN_BULLET:
            OnSpawnBullet(packet);
            break;
         default:
            std::cout << "Recieved a message with an unknown id: "
               << (DefaultMessageIDTypes)packet->data[0];
            break;
         }
      }
   }
}

void Server::UpdateThread(Server * server, RakNet::RakPeerInterface * peerInterface)
{
   const int deltaTime = 17; // time in milliseconds per broadcast
   float timeToNextUpdate = 0; // time until we update all objects
   float updateFrequency = 1.0f; // seconds between updates
   while (true)
   {
      std::vector<int> deathRow;

      float dt = server->GetElapsedTime();
      timeToNextUpdate -= dt;

      for (auto& pair : server->m_gameObjects)
      {
         GameObject& obj = pair.second;
         if (obj.id >= 1000)
         {
            obj.Update(dt);
            if(timeToNextUpdate < 0)
               obj.Write(peerInterface, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

            obj.lifetime -= deltaTime * 0.001f;
            if (obj.lifetime <= 0)
               deathRow.push_back(obj.id);
         }
      }

      if (timeToNextUpdate < 0)
         timeToNextUpdate = updateFrequency;

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

   bs.Write(m_usernameForAddress[packet->systemAddress] + " says: " + str);

   m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Server::ChangeClientName(RakNet::Packet * packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
   std::string ostr = m_usernameForAddress[packet->systemAddress];
   RakNet::RakString str;
   bsIn.Read(str);
   std::cout << "Changed Client's username from " << ostr << " to " << str << std::endl;
   m_usernameForAddress[packet->systemAddress] = str;
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
   m_gameObjects[nextServerID - 1].lifetime = 5.0f;
}

float Server::GetElapsedTime()
{
   LARGE_INTEGER CurrentTime, ElapsedMicroseconds;
   LARGE_INTEGER Frequency;

   QueryPerformanceFrequency(&Frequency);
   QueryPerformanceCounter(&CurrentTime);

   ElapsedMicroseconds.QuadPart = CurrentTime.QuadPart - LastTime.QuadPart;
   LastTime = CurrentTime;

   ElapsedMicroseconds.QuadPart *= 1000000;
   ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
   return ElapsedMicroseconds.QuadPart * 0.000001f;
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
     m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                          RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
      std::this_thread::sleep_for(std::chrono::seconds(10));
   }
}

void Server::Despawn(int id)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_DESPAWN);
   bs.Write(id);
   m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                          RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

   m_gameObjects.erase(id);
}

void Server::SendNewClientID(RakNet::SystemAddress& address)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_SET_CLIENT_ID);
   bs.Write(nextClientID);
   nextClientID++;

  m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED,
                       0, address, false);

  // send all objects to new client
  for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
  {
     GameObject obj = it->second;
     obj.Write(m_peerInterface, address, false);
  }

  // send new to all other clients
  int id = nextClientID - 1;
  GameObject obj;
  obj.data.colour = GameObject::GetColour(id);
  obj.id = id;
  obj.Write(m_peerInterface, address, true);
}

void Server::SpawnObject(glm::vec3 pos, glm::vec3 vel)
{
   m_gameObjects[nextServerID] = GameObject(nextServerID, pos, vel);
   nextServerID++;
}
