#pragma once
#include "DbCommandHandler.h"

class DatabaseInstance
{
    public:
        static DbCommandHandler const& GetDbCommandHandler();
};
