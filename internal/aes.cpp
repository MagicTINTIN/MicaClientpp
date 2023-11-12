#include <stdio.h>
#include <stdlib.h>
#include "aes.h"

const unsigned int DEFAULT_ENCRYPTION_VERSION(2);

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

static uint8 Sbox[256];
static uint8 InvSBox[256];
static uint8 initialized = 0;

static uint8 bw8(uint8 i, int n)
{
    return (i << n) | (i >> (8 - n));
}
static uint32 bw32(uint32 i, int n)
{
    return (i << n) | (i >> (32 - n));
}

static void initSBox()
{
    if (initialized)
        return;
    initialized = 1;
    uint8 p = 1;
    uint8 q = 1;
    Sbox[0] = 0x63;
    InvSBox[0x63] = 0;
    while (1)
    {
        uint8 t = (p & 0x80) >> 7 ? 0x1b : 0;
        p = p ^ (p << 1) ^ t;
        q ^= q << 1;
        q ^= q << 2;
        q ^= q << 4;
        t = (q & 0x80) ? 9 : 0;
        q ^= t;
        uint8 res = q ^ bw8(q, 1) ^ bw8(q, 2) ^ bw8(q, 3) ^ bw8(q, 4) ^ 0x63;
        Sbox[p] = res;
        InvSBox[res] = p;
        if (p == 1)
            return;
    }
}

static uint8 mult(uint8 a, uint8 b)
{
    uint8 p = 0;
    for (uint8 i = 0; i < 8; i++)
    {
        if (b & 1)
            p ^= a;
        uint8 h = a & 0x80;
        a <<= 1;
        if (h)
            a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

static uint8 mult2(uint8 a)
{
    uint8 h = a & 0x80;
    uint8 b = a << 1;
    if (h)
        b ^= 0x1b;
    return b;
}

static uint8 mult3(uint8 a)
{
    return a ^ mult2(a);
}

static uint32 rotate(uint32 a)
{
    return bw32(a, 8);
}

static uint8 rcon(uint32 value)
{
    uint8 c = 1;
    if (!value)
        return 0;
    while (value != 1)
    {
        c = mult2(c);
        value--;
    }
    return c;
}

static uint32 SWord(uint32 i)
{
    uint32 r = 0;
    for (uint8 n = 0; n < 4; n++)
    {
        r |= (Sbox[i & 0xff]) << (n * 8);
        i >>= 8;
    }
    return r;
}

static uint32 InvSWord(uint32 i)
{
    uint32 r = 0;
    for (uint8 n = 0; n < 4; n++)
    {
        r |= (InvSBox[i & 0xff]) << (n * 8);
        i >>= 8;
    }
    return r;
}

static void getKeysV1(uint32 *key, uint32 *output)
{
    for (uint8 i = 0; i < 40; i++)
    {
        if (i < 4)
            output[i] = key[i];
        /*
         * due to an error in the first version of AES,
         * it has never been executed, and won't ever T-T
         * but at least it is still somewhere in the code :p
        else if (i & 0b11 == 0)
            output[i] = output[i - 4] ^ SWord(rotate(output[i - 1])) ^ rcon(i >> 2);
            */
        else
            output[i] = output[i - 4] ^ output[i - 1];
    }
}

static void getKeysV2(uint32 *key, uint32 *output)
{
    for (uint8 i = 0; i < 40; i++)
    {
        if (i < 4)
            output[i] = key[i];
        else if ((i & 0b11) == 0)
            output[i] = output[i - 4] ^ SWord(rotate(output[i - 1])) ^ rcon(i >> 2);
        else
            output[i] = output[i - 4] ^ output[i - 1];
    }
}

static void createMatrix(uint8 *str, uint32 *output)
{
    for (uint8 i = 0; i < 4; i++)
        output[i] = (str[i] << 24) | (str[i + 4] << 16) | (str[i + 8] << 8) | (str[i + 12]);
}

static void addKey(uint32 *matrix, uint32 *keys)
{
    matrix[0] ^= keys[0];
    matrix[1] ^= keys[1];
    matrix[2] ^= keys[2];
    matrix[3] ^= keys[3];
}

static void SBytes(uint32 *matrix)
{
    // return;
    matrix[0] = SWord(matrix[0]);
    matrix[1] = SWord(matrix[1]);
    matrix[2] = SWord(matrix[2]);
    matrix[3] = SWord(matrix[3]);
}

static void InvSBytes(uint32 *matrix)
{
    // return;
    matrix[0] = InvSWord(matrix[0]);
    matrix[1] = InvSWord(matrix[1]);
    matrix[2] = InvSWord(matrix[2]);
    matrix[3] = InvSWord(matrix[3]);
}

static void ShiftRows(uint32 *matrix)
{
    matrix[1] = bw32(matrix[1], 8);
    matrix[2] = bw32(matrix[2], 16);
    matrix[3] = bw32(matrix[3], 24);
}

static void InvShiftRows(uint32 *matrix)
{
    matrix[1] = bw32(matrix[1], 24);
    matrix[2] = bw32(matrix[2], 16);
    matrix[3] = bw32(matrix[3], 8);
}

static void MixColumns(uint32 *matrix)
{
    // return;
    uint8 mat[16], i, b0, b1, b2, b3;
    for (i = 0; i < 4; i++)
    {
        mat[4 * i + 0] = matrix[i] >> 24;
        mat[4 * i + 1] = (matrix[i] >> 16) & 0xff;
        mat[4 * i + 2] = (matrix[i] >> 8) & 0xff;
        mat[4 * i + 3] = matrix[i] & 0xff;
    }
    for (i = 0; i < 4; i++)
    {
        b0 = mat[i];
        b1 = mat[4 + i];
        b2 = mat[8 + i];
        b3 = mat[12 + i];
        mat[0 + i] = mult2(b0) ^ mult3(b1) ^ b2 ^ b3;
        mat[4 + i] = mult2(b1) ^ mult3(b2) ^ b3 ^ b0;
        mat[8 + i] = mult2(b2) ^ mult3(b3) ^ b0 ^ b1;
        mat[12 + i] = mult2(b3) ^ mult3(b0) ^ b1 ^ b2;
    }
    matrix[0] = (mat[0] << 24) | (mat[1] << 16) | (mat[2] << 8) | mat[3];
    matrix[1] = (mat[4] << 24) | (mat[5] << 16) | (mat[6] << 8) | mat[7];
    matrix[2] = (mat[8] << 24) | (mat[9] << 16) | (mat[10] << 8) | mat[11];
    matrix[3] = (mat[12] << 24) | (mat[13] << 16) | (mat[14] << 8) | mat[15];
}

static void InvMixColumns(uint32 *matrix)
{
    // return;
    uint8 mat[16], i, b0, b1, b2, b3;
    for (i = 0; i < 4; i++)
    {
        mat[4 * i + 0] = matrix[i] >> 24;
        mat[4 * i + 1] = (matrix[i] >> 16) & 0xff;
        mat[4 * i + 2] = (matrix[i] >> 8) & 0xff;
        mat[4 * i + 3] = matrix[i] & 0xff;
    }
    for (i = 0; i < 4; i++)
    {
        b0 = mat[i];
        b1 = mat[4 + i];
        b2 = mat[8 + i];
        b3 = mat[12 + i];
        mat[0 + i] = mult(b0, 14) ^ mult(b1, 11) ^ mult(b2, 13) ^ mult(b3, 9);
        mat[4 + i] = mult(b1, 14) ^ mult(b2, 11) ^ mult(b3, 13) ^ mult(b0, 9);
        mat[8 + i] = mult(b2, 14) ^ mult(b3, 11) ^ mult(b0, 13) ^ mult(b1, 9);
        mat[12 + i] = mult(b3, 14) ^ mult(b0, 11) ^ mult(b1, 13) ^ mult(b2, 9);
    }
    matrix[0] = (mat[0] << 24) | (mat[1] << 16) | (mat[2] << 8) | mat[3];
    matrix[1] = (mat[4] << 24) | (mat[5] << 16) | (mat[6] << 8) | mat[7];
    matrix[2] = (mat[8] << 24) | (mat[9] << 16) | (mat[10] << 8) | mat[11];
    matrix[3] = (mat[12] << 24) | (mat[13] << 16) | (mat[14] << 8) | mat[15];
}

static char halfByte2HexChar(uint8 i)
{
    if (i >= 10)
        return 'a' - 10 + i;
    return '0' + i;
}
static uint8 HexChar2HalfByte(char c)
{
    if (c > '9')
        return 10 + c - 'a';
    return c - '0';
}

void AES(unsigned char *text, unsigned char *key, unsigned char *output, int encryptionVersion)
{
    if (encryptionVersion == -1)
        encryptionVersion = DEFAULT_ENCRYPTION_VERSION;

    // separating text in 16 Bytes blocks
    int blockCount = 0;
    int charCount = 0;
    unsigned char *p = text;
    while (*p++)
        charCount++;
    blockCount = charCount / 16 + (charCount % 16 == 0 ? 0 : 1);
    uint8 *textV2 = (uint8 *)calloc(blockCount * 16, 1);
    p = text;
    uint8 *pT = textV2;
    while (*p) // copy text in textV2 with additionnal zeros at the end
        *pT++ = *p++;

    int pOut = 0;
    initSBox();
    uint32 Keys[40];
    if (encryptionVersion < 2)
        getKeysV1((uint32 *)key, Keys);
    else
        getKeysV2((uint32 *)key, Keys);
    
    for (uint16 i = 0; i < blockCount; i++)
    {
        uint32 matrix[4] = {0, 0, 0, 0};
        createMatrix(&textV2[i * 16], matrix);
        addKey(matrix, &Keys[0]);
        for (uint8 i = 1; i <= 8; i++)
        {
            SBytes(matrix);
            ShiftRows(matrix);
            MixColumns(matrix);
            addKey(matrix, &Keys[i * 4]);
        }
        SBytes(matrix);
        ShiftRows(matrix);
        addKey(matrix, &Keys[36]);
        for (uint8 c = 0; c < 4; c++) // pushing matrix to ouput
        {
            for (uint8 l = 0; l < 4; l++)
            {
                uint8 toConvert = (matrix[l] >> (8 * (3 - c))) & 0xff; // to prevent illegal characters
                output[pOut++] = halfByte2HexChar(toConvert >> 4);
                output[pOut++] = halfByte2HexChar(toConvert & 0xf);
            }
        }
    }
    output[pOut] = '\0';
}

int inv_AES(unsigned char *text, unsigned char *key, unsigned char *output, int encryptionVersion)
{
    if (encryptionVersion == -1)
        encryptionVersion = DEFAULT_ENCRYPTION_VERSION;

    // separating text in 16 Bytes blocks
    int blockCount = 0;
    int charCount = 0;
    unsigned char *p = text;
    while (*p++)
        charCount++;
    blockCount = charCount / 32;
    if (charCount % 32 != 0)
    {
        // printf("Error : input text has wrong size : %i\nThe given text is : %s",charCount,text);
        return 1;
    }
    uint8 *textV2 = (uint8 *)calloc(blockCount * 16, 1);
    p = text;
    uint8 *pT = textV2;
    while (*p)
    {
        char c = *p++;
        *pT++ = HexChar2HalfByte(c) * 16 + HexChar2HalfByte(*p++);
    }

    int pOut = 0;
    initSBox();
    uint32 Keys[40];
    if (encryptionVersion < 2)
        getKeysV1((uint32 *)key, Keys);
    else
        getKeysV2((uint32 *)key, Keys);

    for (uint16 i = 0; i < blockCount; i++)
    {
        uint32 matrix[4] = {0, 0, 0, 0};
        createMatrix(&textV2[i * 16], matrix);
        addKey(matrix, &Keys[36]);
        InvShiftRows(matrix);
        InvSBytes(matrix);
        for (uint8 i = 1; i <= 8; i++)
        {
            addKey(matrix, &Keys[36 - i * 4]);
            InvMixColumns(matrix);
            InvShiftRows(matrix);
            InvSBytes(matrix);
        }
        addKey(matrix, &Keys[0]);
        for (uint8 c = 0; c < 4; c++) // pushing matrix to ouput
        {
            for (uint8 l = 0; l < 4; l++)
            {
                output[pOut++] = (matrix[l] >> (8 * (3 - c))) & 0xff;
            }
        }
    }
    output[pOut] = '\0';
    return 0;
}