#pragma once

#ifdef NDEBUG
#define DEBUG_LOG [] (...) { }
#else
#define DEBUG_LOG printf
#endif

enum ChatId
{
    CHAT_GLOBAL = 1,
    CHAT_LOCAL,
    CHAT_WHISPER,
    CHAT_WHISPER_RESPONSE
};
