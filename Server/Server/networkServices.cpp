#include "networkServices.h"

// Sends message to active socket
int NetworkServices::sendMessage(SOCKET const& curSocket, uint8_t const* message, int messageSize)
{
    return send(curSocket, (char const*)message, messageSize, 0);
}

// Receive message from active socket
int NetworkServices::receiveMessage(SOCKET const& curSocket, char* buffer, int bufSize)
{
    return recv(curSocket, buffer, bufSize, MSG_DONTWAIT);
}
