﻿
namespace Client.Logic.Security
{
    /// <summary>
    /// TODO: move this to app config or somewhere
    /// </summary>
    public static class RSAKey
    {
        public static readonly byte[] Modulus =
        {
            0xcb,0xc0,0xdb,0xbf,0xca,0x6b,0xa4,0x9f,0xf4,0x90,0xa8,0x65,0x19,0xe2,
            0x58,0xa3,0x3a,0x36,0xb7,0xad,0x04,0x1b,0xc2,0xf4,0xe7,0xad,0x60,0xd7,0x74,
            0x76,0xf4,0xbb,0xcc,0x47,0x98,0x72,0xbc,0x66,0x65,0x18,0x9a,0x66,0x9f,0xae,
            0x7e,0x03,0x8c,0x47,0x5c,0x89,0xc9,0x62,0x23,0xee,0x2b,0x3a,0xca,0x52,0x70,
            0x29,0x78,0xb6,0x7e,0xbf,0x0d,0x47,0xa1,0xc3,0x80,0x66,0xd5,0x8a,0xd3,0x3d,
            0xeb,0xdb,0xab,0x80,0xf2,0x44,0x18,0x38,0xa5,0xfd,0x42,0xdf,0xc3,0x6c,0x27,
            0x6f,0xb0,0x5f,0x70,0x17,0xc8,0x11,0x2b,0x4a,0x6d,0xc7,0xd3,0x3a,0x47,0x5d,
            0xcd,0xbc,0x1c,0x6f,0x55,0x00,0x29,0x7d,0xcf,0x63,0x07,0xd9,0xd3,0xce,0x98,
            0x4e,0x80,0xce,0x09,0x88,0x46,0x2f,0x95,0x5f,0x05,0x2e,0x18,0x3d,0xc9,0x9a,
            0x2f,0x31,0x87,0x7a,0x5b,0x07,0xf7,0x6a,0x85,0xca,0xf3,0xdd,0x55,0x43,0x14,
            0xeb,0xfe,0x44,0x18,0xa2,0xad,0x58,0xa7,0xee,0xbc,0x8b,0xa4,0xf8,0x9c,0xda,
            0xbb,0x35,0x60,0xbe,0x50,0xd5,0x63,0x40,0x7c,0x4d,0x40,0xc4,0x0c,0xcd,0x4e,
            0xa5,0x58,0x25,0xfc,0xcf,0x28,0x37,0xc8,0x7f,0x3c,0x38,0x04,0x24,0x51,0x72,
            0x17,0x29,0xf4,0x3e,0x36,0x4b,0xd8,0x43,0x57,0x31,0x68,0xca,0x15,0x3e,0x96,
            0x3b,0xdb,0xe7,0x95,0xb6,0x12,0xb3,0xa4,0xc7,0xb2,0x1e,0x40,0x67,0xed,0xc8,
            0xae,0x9a,0x4e,0x6a,0x80,0xb4,0xc4,0x24,0x07,0xd6,0x66,0x97,0xb8,0x7f,0x87,
            0x85,0x66,0x3f,0xd1,0x73,0xc6,0x3c,0x26,0x3d,0x88,0x4b,0x99,0x15,0x3e,0x35,
            0x32,0xc1
        };

        public static readonly byte[] Exponent = { 1, 0, 1 };
    }
}