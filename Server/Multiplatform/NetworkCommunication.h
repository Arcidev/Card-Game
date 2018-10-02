#pragma once

#ifdef _WIN32
#include "winsock2.h"
#include "ws2tcpip.h"
#pragma comment (lib, "Ws2_32.lib")
#define InitWinsock(word, data) WSAStartup(word, data)
#define CloseWinsock WSACleanup
#define GetSockError() WSAGetLastError()
#define IoctlSocket ioctlsocket
#endif // !_WIN32

#ifdef __unix__
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "netdb.h"
#include "netinet/tcp.h"
#include "errno.h"
#include "unistd.h"
#include <cstring>
#define SOCKET int
#define WSADATA int
#define MAKEWORD(a,b) ((unsigned short)(((unsigned char)(a))|(((unsigned short)((unsigned char)(b)))<<8)))
#define InitWinsock [] (...) { return 0; }
#define CloseWinsock [] () { }
#define closesocket close
#define GetSockError() errno
#define ZeroMemory(destination, length) memset(destination, 0, length)
#define IoctlSocket ioctl
constexpr int SD_BOTH = 2;
constexpr int INVALID_SOCKET = -1;
constexpr int SOCKET_ERROR = -1;
#endif // !__unix__
