#include <iostream>
#include "Game.h"
#include "ConnectedUser.h"
#include "NetworkServices.h"
#include "PacketHandlers/Packet.h"
#include "Player.h"
#include "ServerNetwork.h"
#include "StaticHelper.h"
#include "../Shared/SharedDefines.h"

// Creates network
ServerNetwork::ServerNetwork() : m_lastPlayer(nullptr), m_listenSocket(INVALID_SOCKET), m_shuttingDown(false)
{
    // create WSADATA object
    WSADATA wsaData;

    // address info for the server to listen to
    struct addrinfo *result = nullptr;
    struct addrinfo hints;

    // Initialize Winsock
    int iResult = InitWinsock(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        exit(1);
    }

    // set address information
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
    hints.ai_flags = AI_PASSIVE;
    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);

    if (iResult != 0)
    {
        std::cerr << "Getaddrinfo failed with error: " << iResult << std::endl;
        CloseWinsock();
        exit(1);
    }

    // Create a SOCKET for connecting to server
    m_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (m_listenSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket failed with error: " << GetSockError() << std::endl;
        freeaddrinfo(result);
        CloseWinsock();
        exit(1);
    }

    // Set the mode of the socket to be blocking
    u_long iMode = 0;
    iResult = IoctlSocket(m_listenSocket, FIONBIO, &iMode);

    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "IoctlSocket failed with error: " << GetSockError() << std::endl;
        shutdown(m_listenSocket, SD_BOTH);
        CloseWinsock();
        exit(1);
    }

    // Setup the TCP listening socket
    iResult = bind(m_listenSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Bind failed with error: " << GetSockError() << std::endl;
        freeaddrinfo(result);
        shutdown(m_listenSocket, SD_BOTH);
        CloseWinsock();
        exit(1);
    }

    // no longer need address information
    freeaddrinfo(result);

    // start listening for new clients attempting to connect
    iResult = listen(m_listenSocket, SOMAXCONN);

    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Listen failed with error: " << GetSockError() << std::endl;
        shutdown(m_listenSocket, SD_BOTH);
        CloseWinsock();
        exit(1);
    }
}

// Removes all resources
ServerNetwork::~ServerNetwork()
{
    DEBUG_LOG("Destroying server network\r\n");

    GetLocker().lock();
    m_shuttingDown = true;
    GetLocker().unlock();

    for (auto const& user : m_users)
    {
        user.second.first->Disconnect();
        user.second.second->join();
        delete user.second.second;
        if (user.second.first->GetPlayer())
            user.second.first->GetPlayer()->GetGame()->RemovePlayer(user.second.first->GetId());
        delete user.second.first;
    }

    DEBUG_LOG("Server network cleared\r\n");
}

// Accepts new connections
bool ServerNetwork::AcceptNewClient(unsigned int const& id, AchievementManager* achievementMgr)
{
    // if client is waiting, accept the connection and save the socket
    SOCKET clientSocket = accept(m_listenSocket, nullptr, nullptr);
    if (clientSocket != INVALID_SOCKET)
    {
        ConnectedUser* user = new ConnectedUser(id, clientSocket, this, achievementMgr);
        std::thread* t = new std::thread(&ServerNetwork::handlePlayerNetwork, user);

        // insert new client into session id table
        m_users.insert(std::make_pair(id, std::make_pair(user, t)));
        return true;
    }

    return false;
}

void ServerNetwork::handlePlayerNetwork(ConnectedUser* user)
{
    ServerNetwork* network = user->GetNetwork();
    int userId = user->GetId();
    srand((intptr_t)user & (int)-1); // set seed from player memory address

    while (!network->IsShuttingDown())
    {
        char networkData[MAX_PACKET_SIZE];

        // get data for that client
        int dataLength = network->ReceiveData(user, networkData);
        if (!dataLength)
        {
            if (!network->IsShuttingDown())
            {
                std::mutex& locker = network->GetLocker();
                locker.lock();

                try
                {
                    if (!network->IsShuttingDown())
                    {
                        user->Disconnect();
                        UserMap::iterator iter = network->GetUsers().find(userId);
                        if (iter != network->GetUsers().end())
                        {
                            iter->second.second->detach();
                            delete iter->second.second;

                            network->GetUsers().erase(iter);
                        }

                        if (user->GetPlayer() && user->GetPlayer()->GetGame()->IsEmpty())
                            delete user->GetPlayer()->GetGame();
                    }
                }
                catch (std::exception const& ex)
                {
                    std::cerr << "Error while disconnecting player: " << ex.what() << std::endl;
                }
                catch(...)
                {
                    std::cerr << "Unknown failure occured while disconnecting player." << std::endl;
                    // Prevent deadlock
                }

                locker.unlock();
            }
            break;
        }

        // invalid packet received
        if (dataLength < 2)
            continue;

        user->ReceivePacket(dataLength, networkData);
    }

    DEBUG_LOG("Thread of player %d has ended.\r\n", userId);
}

// receive incoming data
int ServerNetwork::ReceiveData(ConnectedUser const* user, char* recvbuf) const
{
    return NetworkServices::ReceiveMessage(user->GetSocket(), recvbuf, MAX_PACKET_SIZE);
}

// Broadcasts packet to all clients
void ServerNetwork::BroadcastPacket(Packet const& packet) const
{
    for (auto const& user : m_users)
        user.second.first->SendPacket(packet);
}

// Sends packet to player searched by name
bool ServerNetwork::SendPacketToPlayer(std::string_view playerName, Packet const& packet) const
{
    for (auto const& user : m_users)
    {
        if (StaticHelper::CompareStringCaseInsensitive(user.second.first->GetName(), playerName))
        {
            user.second.first->SendPacket(packet);
            return true;
        }
    }

    return false;
}

// Set last player to null if this player was the one last connected
void ServerNetwork::OnPlayerDisconnected(Player const* player)
{
    if (m_lastPlayer && (player->GetId() == m_lastPlayer->GetId()))
        m_lastPlayer = nullptr;
}

Game* ServerNetwork::GetGameForPlayer()
{
    Game* game = nullptr;
    if (m_lastPlayer)
    {
        game = m_lastPlayer->GetGame();
        if (game->IsFull())
            game = new Game();
    }
    else
        game = new Game();

    return game;
}
