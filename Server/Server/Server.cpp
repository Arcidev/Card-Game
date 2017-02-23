#include "time.h" 
#include "ServerGame.h"
#include "DataHolder.h"

int main()
{
    srand((unsigned int)time(nullptr)); // set seed on startup
    DataHolder::LoadData(); // loads data from database
    ServerGame server;
    server.Loop();

	return 0;
}
