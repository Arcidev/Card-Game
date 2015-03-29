#include "serverGame.h"
#include "DataHolder.h"

int main()
{
    DataHolder::LoadData();
    ServerGame server;
    server.Loop();

	return 0;
}
