
#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <chrono>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameMessages.h"

std::map<RakNet::SystemAddress, std::string> usernameForAddress;

void handleNetworkMessages(RakNet::RakPeerInterface* peerInterface);
void forwardMessage(RakNet::Packet * packet, RakNet::RakPeerInterface * peerInterface);
void changeClientName(RakNet::Packet * packet);
void sendClientPing(RakNet::RakPeerInterface* peerInterface);
void sendNewClientID(RakNet::RakPeerInterface* pPeerInterface, RakNet::SystemAddress& address);

int nextClientID = 1;


/////////////////////////////////////////////
int main()
{
   RakNet::RakPeerInterface* peerInterface = nullptr;
   const unsigned int short PORT = 5456;


   std::cout << "Starting up the server..." << std::endl;

   //get an instance of the peer interface
   peerInterface = RakNet::RakPeerInterface::GetInstance();

   //create a descriptor for this socket
   RakNet::SocketDescriptor sd(PORT, 0);

   peerInterface->Startup(32, &sd, 1);
   peerInterface->SetMaximumIncomingConnections(32);
   std::cout << "Server started." << std::endl;

   std::thread pingThread(sendClientPing, peerInterface);

   handleNetworkMessages(peerInterface);
	
	return 0;
}

/////////////////////////////////////
// Handles each packet recieved
/////////////////////////////////////
void handleNetworkMessages(RakNet::RakPeerInterface* peerInterface)
{
   RakNet::Packet* packet = nullptr;
   
   while (true)
   {
      for (packet = peerInterface->Receive(); packet;
           peerInterface->DeallocatePacket(packet),
           packet = peerInterface->Receive())
      {
         switch (packet->data[0])
         {
         case ID_NEW_INCOMING_CONNECTION:
            usernameForAddress[packet->systemAddress] = "ANON";
            std::cout << "A connection is incoming. \n";
            sendNewClientID(peerInterface, packet->systemAddress);
            break;
         case  ID_DISCONNECTION_NOTIFICATION:
            std::cout << "A client has disconnected. \n";
            break;
         case ID_CONNECTION_LOST:
            std::cout << "A client lost connection. \n";
            break;
         case ID_CLIENT_CHANGE_USERNAME:
            changeClientName(packet);
         case ID_CLIENT_CHAT_MESSAGE:
            forwardMessage(packet, peerInterface);
            break;
         case ID_CLIENT_OBJECT_DATA:
         {
            RakNet::BitStream bs(packet->data, packet->length, false);
            peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                       packet->systemAddress, true);
            break;
         }
         default:
            std::cout << "Recieved a message with an unknown id: "
               << packet->data[0];
            break;
         }
      }
   }
}

void forwardMessage(RakNet::Packet * packet, RakNet::RakPeerInterface * peerInterface)
{
   std::cout << "Forwarding message from client. \n";
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

   std::string str;
   bsIn.Read(str);
   RakNet::BitStream bs;

   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CHAT_MESSAGE);

   bs.Write(usernameForAddress[packet->systemAddress] + ": " + str);

   peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                       RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void changeClientName(RakNet::Packet * packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

   std::string ostr = usernameForAddress[packet->systemAddress];
   RakNet::RakString str;
   bsIn.Read(str);
   std::cout << "Changed Client's username from " << ostr << " to " << str << std::endl;
   usernameForAddress[packet->systemAddress] = str;
}

/////////////////////////////////////////////
// pings the clients once per second
/////////////////////////////////////////////
void sendClientPing(RakNet::RakPeerInterface* peerInterface)
{
   while (true)
   {
      RakNet::BitStream bs;
      bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_TEXT_MESSAGE);
      bs.Write("Ping!");
      peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                          RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
      std::this_thread::sleep_for(std::chrono::seconds(10));
   }
}

void sendNewClientID(RakNet::RakPeerInterface* peerInterface, RakNet::SystemAddress& address)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_SET_CLIENT_ID);
   bs.Write(nextClientID);
   nextClientID++;

   peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}
