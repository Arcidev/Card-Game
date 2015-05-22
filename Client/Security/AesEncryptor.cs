using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;
using System.IO;

namespace Client.Security
{
    public static class AesEncryptor
    {
        private static Aes aes;
        private static byte[] ivec = { 255, 254, 253, 252, 251, 250, 249, 248, 60, 61, 62, 63, 64, 65, 66, 67 };
        private static byte[] key = new byte[16];

        public static byte[] Key { get { return key; } }

        // Incializes AES instance
        public static void Inicialize()
        {
            if (aes != null)
                aes.Clear();

            aes = Aes.Create();
            using (var rng = new RNGCryptoServiceProvider())
            {
                rng.GetBytes(key);
            }

            aes.IV = ivec;
            aes.Key = key;
            aes.Padding = PaddingMode.Zeros;
        }

        // Encrypts data
        public static byte[] Encrypt(string toEncrypt)
        {
            return Encrypt(System.Text.Encoding.ASCII.GetBytes(toEncrypt));
        }

        // Encrypts data
        public static byte[] Encrypt(byte[] toEncrypt)
        {
            if (aes == null)
                return null;

            ICryptoTransform encryptor = aes.CreateEncryptor();
            byte[] encodedText = null;

            using (MemoryStream msEncrypt = new MemoryStream())
            {
                using (CryptoStream csEncrypt = new CryptoStream(msEncrypt, encryptor, CryptoStreamMode.Write))
                {
                    using (BinaryWriter swEncrypt = new BinaryWriter(csEncrypt))
                    {
                        // Write all data to the stream.
                        swEncrypt.Write(toEncrypt);
                    }

                    encodedText = msEncrypt.ToArray();
                }
            }

            return encodedText;
        }

        // Decrypts data
        public static byte[] Decrypt(string toDecrypt)
        {
            return Decrypt(System.Text.Encoding.ASCII.GetBytes(toDecrypt), toDecrypt.Length);
        }

        // Decrypts data
        public static byte[] Decrypt(byte[] toDecode, int length)
        {
            if (aes == null)
                return null;

            ICryptoTransform decryptor = aes.CreateDecryptor(aes.Key, aes.IV);
            byte[] decodedText = null;

            using (MemoryStream msDecrypt = new MemoryStream(toDecode.Take(length).ToArray()))
            {
                using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Read))
                {
                    using (BinaryReader srDecrypt = new BinaryReader(csDecrypt))
                    {
                        // Read the decrypted bytes from the decrypting stream
                        decodedText = srDecrypt.ReadBytes(length);
                    }
                }
            }

            return decodedText;
        }

        // Clears allocated resources
        public static void Clear()
        {
            if (aes != null)
            {
                aes.Clear();
                aes = null;
            }
        }
    }
}
