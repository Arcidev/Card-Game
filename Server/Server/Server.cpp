#include "time.h" 
#include "ServerGame.h"
#include "DataHolder.h"
#include "SignalHandler.h"

int main()
{
    srand((unsigned int)time(nullptr)); // set seed on startup
    DataHolder::LoadData(); // loads data from database
    SignalHandler::Initialize();
    ServerGame server;
    server.Loop();

	return 0;
}
