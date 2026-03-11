#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define BASE (UINT_MAX + 1ULL)

typedef struct BigInt
{
    int msd; // most significant digit
    unsigned int *digits;
} BigInt;

void bigint_init(BigInt *num)
{
    num->msd = 0;
    num->digits = malloc(sizeof(unsigned int));
    num->digits[0] = 0;
}

void bigint_free(BigInt *num)
{
    num->msd = 0;
    free(num->digits);
    num->digits = NULL;
}

void bigint_from_int(BigInt *num, int int_num)
{
    unsigned int *tmp = realloc(num->digits, sizeof(unsigned int) * 1);
    if (tmp == NULL) return;
    num->digits = tmp;
    
    num->digits[0] = 1;
    num->msd = int_num;
}
unsigned int min(unsigned int a, unsigned int b)
{
    if (a >= b)
        return b;
    else
        return a;
}

unsigned int max(unsigned int a, unsigned int b)
{
    if (a >= b)
        return a;
    else
        return b;
}

void bigint_add(BigInt* a, BigInt* b)
{
    unsigned int min_len = min(a->digits[0], b->digits[0]);
    unsigned int max_len = max(a->digits[0], b->digits[0]);
    
    unsigned int a_is_max_len = max_len == a->digits[0];

    unsigned int *tmp = realloc(a->digits, sizeof(unsigned int) * (max_len + 2));
    if (tmp == NULL) return;
    a->digits = tmp;

    unsigned int carry = 0;
    unsigned long long digit_sum = 0;

    for (size_t i = 1; i <= max_len; i++)
    {   
        if (i <= min_len)
        {
            digit_sum = a->digits[i] + b->digits[i] + carry;
            carry = 0;
            if (digit_sum >= BASE)
            {
                digit_sum %= BASE;
                carry = 1;
            }
            a->digits[i] = digit_sum;
        }else
        {
            digit_sum = a->digits[i] + b->digits[i] + carry;
            carry = 0;
            
            if (digit_sum >= BASE)
            {
                digit_sum %= BASE;
                carry = 1;
            }

            if (!a_is_max_len) // a len < b len
            {
                realloc(a->digits, sizeof(unsigned int)*(a->digits[0]+2));
            }
            a->digits[i] = digit_sum;
            
        }
        
    }
    
}