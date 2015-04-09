#include "time.h" 
#include "serverGame.h"
#include "DataHolder.h"

int main()
{
    srand((unsigned int)time(nullptr)); // set seed on startup
    DataHolder::LoadData();
    ServerGame server;
    server.Loop();

	return 0;
}
