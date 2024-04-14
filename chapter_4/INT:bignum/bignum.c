#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#define TEST

#define DIGIT_MASK (0x000000ff)
#define DIGITS_IN_BYTE (2u) /*assuming we can put one decimal digit in 4 bits*/
#define DIGITS_IN_UINT32 (sizeof(uint32_t) * DIGITS_IN_BYTE)
#define GET_32BIT_DATA_COUNT(numOfDigits) ((numOfDigits / DIGITS_IN_UINT32) + (((numOfDigits % DIGITS_IN_UINT32) == 0) ? 0 : 1))
// (numOfDigits / DIGITS_IN_UINT32)
#define GET_PACKED_DATA(digit, bitShift) ((uint32_t)((digit - '0') & 0xf) << bitShift)
#define GET_CHAR_FROM_4BITS(data) ((char)((data & 0xf) + '0'))

typedef struct
{
    uint32_t *num;
    uint8_t sign;
    size_t numSize;
} big_int_t;

void bignum_show(char *bigNum)
{
    printf("%s\n", bigNum);
}

/**
 * @brief Turns text representation of a number
 *  into big numbers type (big_int_t)
 *
 * @param number string representation of a bignum
 * @return big_int_t
 */
big_int_t *bignum_strToBig(char *number)
{
    size_t numOfDigits = strlen(number);
    big_int_t *retVal = (big_int_t *)malloc(sizeof(big_int_t));
    retVal->numSize = numOfDigits;

    retVal->num = (uint32_t *)malloc(sizeof(uint32_t) * GET_32BIT_DATA_COUNT(numOfDigits)); //(numOfDigits / DIGITS_IN_UINT32) + (((numOfDigits % DIGITS_IN_UINT32) == 0) ? 0 : 1));

    for (size_t i = 0; i < numOfDigits; i++)
    {
        retVal->num[i / DIGITS_IN_UINT32] |= GET_PACKED_DATA(number[i], (i * 4) % 32);
    }
    return retVal;
}

char *bignum_bigToStr(big_int_t bigNum)
{
    char *retVal = malloc((sizeof(char) * bigNum.numSize) + 1);

    for (size_t i = 0; i < bigNum.numSize; i++)
    {
        retVal[i] = GET_CHAR_FROM_4BITS(bigNum.num[(i * 4) / 32] >> (i * 4));
    }
    retVal[bigNum.numSize] = '\0';

    return retVal;
}

big_int_t bignum_add(big_int_t a, big_int_t b)
{
}

big_int_t bignum_multiply(big_int_t a, big_int_t b)
{
}

#ifdef TEST

/* TEST */
int main(int argc, char const *argv[])
{
    big_int_t *myBigNum;
    char *myStr;
    myBigNum = bignum_strToBig("123456789");
    myStr = bignum_bigToStr(*myBigNum);
    bignum_show(myStr);

    return 0;
}

#endif