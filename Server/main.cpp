
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
void sendClientPing(RakNet::RakPeerInterface* peerInterface);

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
            usernameForAddress[packet->systemAddress] = "";
            std::cout << "A connection is incoming. \n";
            break;
         case  ID_DISCONNECTION_NOTIFICATION:
            std::cout << "A client has disconnected. \n";
            break;
         case ID_CONNECTION_LOST:
            std::cout << "A client lost connection. \n";
            break;
         case ID_CLIENT_CHANGE_USERNAME:
         {
            RakNet::BitStream bsIn(packet->data, packet->length, false);
            bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

            std::string ostr = usernameForAddress[packet->systemAddress];
            RakNet::RakString str;
            bsIn.Read(str);
            std::cout << "Changed Client's username from " << ostr << " to " << str << std::endl;
            usernameForAddress[packet->systemAddress] = str;
         }
         case ID_CLIENT_CHAT_MESSAGE:
         {
            std::cout << "Forwarding message from client. \n";
            RakNet::BitStream bsIn(packet->data, packet->length, false);
            peerInterface->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
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
