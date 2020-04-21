using System;

namespace Client.Logic
{
    public class Player
    {
        public UInt32 Id { get; }

        public string Name { get; }

        public Player(UInt32 id, string name)
        {
            Id = id;
            Name = name;
        }
    }
}
