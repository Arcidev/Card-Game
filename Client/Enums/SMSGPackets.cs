using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardGameWPF.Enums
{
    public enum SMSGPackets
    {
        SMSG_INIT_RESPONSE = 1,
        SMSG_AVAILABLE_CARDS,
        SMSG_CHAT_MESSAGE,
        SMSG_WHISPER_FAILED,
        SMSG_SELECT_CARDS_FAILED,
        SMSG_SELECT_CARDS_WAIT_FOR_ANOTHER_PLAYER,
        SMSG_SELECT_CARDS,
        SMSG_DECK_CARDS,
        SMSG_ACTIVE_PLAYER
    }
}
