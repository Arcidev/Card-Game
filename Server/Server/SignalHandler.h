#pragma once
#include <functional>
#include <map>

typedef std::map<int, std::function<bool()>> SignalHandlersMap;

class SignalHandler
{
    private:
        static SignalHandlersMap m_handlers;
        static void signalControlHandler(int signal);

    public:
        static void RegisterMethod(int signal, std::function<bool()> func);
};

