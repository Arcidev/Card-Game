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
#include <cstring>
#define SOCKET int
#define WSADATA int
#define SD_BOTH 2
#define InitWinsock(word, data) 0
#define CloseWinsock [] () { }
#define GetSockError() errno
#define ZeroMemory(destination, length) memset((destination),0,(length))
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#define IoctlSocket ioctl
#endif // !__unix__
