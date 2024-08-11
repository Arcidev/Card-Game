#pragma once
#include <functional>
#include <map>

class SignalHandler
{
    private:
        typedef std::map<int, std::function<bool()>> SignalHandlersMap;

        static SignalHandlersMap m_handlers;
        static void signalControlHandler(int signal);

    public:
        static void RegisterMethod(int signal, std::function<bool()> func);
};

