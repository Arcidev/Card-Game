#include "ServerGame.h"
#include "DataHolder.h"
#include "SignalHandler.h"

int main()
{
    DataHolder::LoadData();
    ServerGame server;
    server.Loop();

    return 0;
}
