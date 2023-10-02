#include <stdio.h>
#include <stdint.h>
#include <string.h>

int isUTF8ContinuationByte(unsigned char c)
{
    // In UTF-8, continuation bytes have the form 10xxxxxx
    return (c & 0xC0) == 0x80;
}

int decodeUTF8Character(unsigned char *data, unsigned char *out, size_t *index, size_t *outIndex, uint32_t *value)
{
    unsigned char tmp[4];
    // Check if the byte at the given index is a start byte for a multibyte character
    if ((data[*index] & 0xC0) == 0xC0)
    {
        // Count the number of leading '1' bits to determine the character length
        int leadingOnes = 0;
        unsigned char mask = 0x80;

        // set a temporary variable to add it to out if the char is a valid UTF-8 char
        tmp[0] = data[*index];

        while ((data[*index] & mask) == mask)
        {
            leadingOnes++;
            mask >>= 1;
        }

        // In UTF-8, multibyte characters have the form 110xxxxx, 1110xxxx, 11110xxx, etc.
        if (leadingOnes > 1)
        {
            // Initialize the character value
            *value = data[*index] & (0xFF >> (leadingOnes + 1));

            // Validate and decode the remaining bytes
            for (int i = 1; i < leadingOnes; i++)
            {
                (*index)++;

                tmp[i] = data[*index];
                if (isUTF8ContinuationByte(data[*index]))
                {
                    *value <<= 6;
                    *value |= (data[*index] & 0x3F);
                }
                else
                {
                    return 0;
                }
            }

            //if (*value >= 0x80 && *value <= 0xD7FF && *value != 0xFFFE && *value != 0xFFFF)
            if (*value >= 0x80 && *value <= 0xFF && *value != 0xFFFE && *value != 0xFFFF)
            {
                for (size_t i = 0; i < leadingOnes; i++)
                {
                    out[(*outIndex)++] = tmp[i];
                }
                
                return 1;
            }
            else
                return 0;
        }
    }
    // simple ascii
    else if (data[*index] >= 0x20 && data[*index] <= 0x7E)
    {
        out[(*outIndex)++] = data[*index];
        return 1;
    }

    // Not a valid character at all
    return 0;
}

int main()
{
    unsigned char data[] = "C'est une chaîne de caractère UTF-8 avec des ⋅ et â ou d'autres caractères à la con";
    size_t index = 0;
    uint32_t characterValue;
    size_t dataIndex = 0; // Index for the modified data

    // Create a buffer to store the modified data without valid UTF-8 characters
    unsigned char modifiedData[sizeof(data)];

    while (index < sizeof(data))
    {
        if (decodeUTF8Character(data, modifiedData, &index, &dataIndex, &characterValue))
        {
            printf("v ");
            // Valid UTF-8 Character Found, skip it
        }
        else
        {
            printf("i ");
            // Not a valid extended character, copy it to modifiedData
            //modifiedData[dataIndex++] = ' '; // data[index++]
            
        }
        index++;
    }

    // Null-terminate the modifiedData to make it a valid C string
    modifiedData[dataIndex] = '\0';

    printf("\nOriginal string: %s\n", data);
    printf("Modified string: %s\n", modifiedData);

    return 0;
}
