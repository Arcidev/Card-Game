using System;
using System.ComponentModel;

namespace Client.Logic.Enums
{
    public static class EnumExtension
    {
        public static string GetDescription<TEnum>(this TEnum enumerationValue) where TEnum : Enum
        {
            var type = enumerationValue.GetType();

            // Tries to find a DescriptionAttribute for a potential friendly name for the enum
            var memberInfo = type.GetMember(enumerationValue.ToString());
            if (memberInfo?.Length > 0)
            {
                var attrs = memberInfo[0].GetCustomAttributes(typeof(DescriptionAttribute), false);
                if (attrs?.Length > 0)
                {
                    // Pull out the description value
                    return ((DescriptionAttribute)attrs[0]).Description;
                }
            }
            // If we have no description attribute, just return the ToString of the enum
            return enumerationValue.ToString();
        }
    }
}
