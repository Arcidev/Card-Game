#pragma once

#ifdef _WIN32
#include "Windows.h"
#define SIGNAL DWORD
#define SIGNAL_CTRL_C CTRL_C_EVENT
#endif // !_WIN32

#ifdef __unix__
#define SIGNAL int
#define SIGNAL_CTRL_C SIGINT
#endif // !__unix__
