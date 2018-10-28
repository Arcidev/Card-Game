#include "ServerGame.h"
#include "DataHolder.h"
#include "SignalHandler.h"

int main()
{
    DataHolder::LoadData(); // loads data from database
    ServerGame server;
    server.Loop();

    return 0;
}
