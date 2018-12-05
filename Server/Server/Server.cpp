#include "ServerGame.h"
#include "DataHolder.h"
#include "SignalHandler.h"

int main()
{
    if (!DataHolder::LoadData()) // loads data from database
        return 1;

    ServerGame server;
    server.Loop();

    return 0;
}
