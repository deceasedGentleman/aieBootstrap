#pragma once

#include "MessageIdentifiers.h"

enum GameMessages
{
   ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
   ID_SERVER_SET_CLIENT_ID,
   ID_CLIENT_CHAT_MESSAGE,
   ID_CLIENT_CHANGE_USERNAME
};
