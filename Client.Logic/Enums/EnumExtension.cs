using System;
using System.ComponentModel;
using System.Reflection;

namespace Client.Logic.Enums
{
    public static class EnumExtension
    {
        public static string GetDescription<TEnum>(this TEnum enumerationValue) where TEnum : struct
        {
            Type type = enumerationValue.GetType();
            if (!type.IsEnum)
            {
                throw new ArgumentException("EnumerationValue must be of Enum type", "enumerationValue");
            }

            // Tries to find a DescriptionAttribute for a potential friendly name
            // for the enum
            MemberInfo[] memberInfo = type.GetMember(enumerationValue.ToString());
            if (memberInfo?.Length > 0)
            {
                var attrs = memberInfo[0].GetCustomAttributes(typeof(DescriptionAttribute), false);

                if (attrs != null && attrs.Length > 0)
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
