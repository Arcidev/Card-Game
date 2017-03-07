#pragma once
#include <functional>
#include <map>
#include "../Multiplatform/NetworkCommunication.h"
#include "../Multiplatform/Signals.h"

typedef std::map<SIGNAL, std::function<bool()>> SignalHandlersMap;

class SignalHandler
{
    private:
        static SignalHandlersMap m_handlers;
        static int signalControlHandler(SIGNAL signal);

    public:
        static void Initialize();
        static void RegisterMethod(SIGNAL signal, std::function<bool()> func) { m_handlers.insert(std::make_pair(signal, func)); }
};

