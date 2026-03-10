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

void bigint_add(BigInt* a, BigInt* b)
{
    unsigned int smaller_length = a->digits[0] > b->digits[0] ? b->digits[0] : a->digits[0];
    
    
    for (size_t i = 0; i < smaller_length-1; i++)
    {
        if ()
        {
            /* code */
        }
        
    }
    
}