#include "SignalHandler.h"
#include "../Shared/SharedDefines.h"

SignalHandlersMap SignalHandler::m_handlers;

#ifdef _WIN32
void SignalHandler::Initialize()
{
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)signalControlHandler, true);
}
#endif

#ifdef __unix__
void SignalHandler::Initialize()
{

}
#endif // !__unix__

bool SignalHandler::signalControlHandler(SIGNAL signal)
{
    DEBUG_LOG("Calling signal handler from signal %lu\r\n", signal);
    SignalHandlersMap::const_iterator iter = m_handlers.find(signal);
    if (iter == m_handlers.end())
        return false;

    DEBUG_LOG("Running registered handler for signal %lu\r\n", signal);
    return iter->second();
}

