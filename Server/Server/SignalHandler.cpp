#include <csignal>
#include "SignalHandler.h"
#include "../Shared/SharedDefines.h"

SignalHandlersMap SignalHandler::m_handlers;

void SignalHandler::RegisterMethod(int signal, std::function<bool()> func)
{
    if (func)
        m_handlers.insert(std::make_pair(signal, func));

    std::signal(signal, [](int signal) { signalControlHandler(signal); });
}

void SignalHandler::signalControlHandler(int signal)
{
    DEBUG_LOG("Calling signal handler from signal %lu\r\n", signal);
    SignalHandlersMap::const_iterator iter = m_handlers.find(signal);
    if (iter == m_handlers.end())
        exit(signal);

    DEBUG_LOG("Running registered handler for signal %lu\r\n", signal);
    if (!iter->second())
        exit(signal);
}

