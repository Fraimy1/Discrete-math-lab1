#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//TODO: add more safety checks for ->digits == NULL and a/b == NULL

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

unsigned int bigint_get_digit(BigInt *num, unsigned int original_len, size_t i)
{
    if (i <= original_len)
    {
        return num->digits[i];
    }
    else if (i == original_len + 1)
    {
        return (unsigned int)num->msd;
    }
    else
    {
        return 0;
    }
}

void bigint_sub(BigInt* a, BigInt* b)
{

}

void bigint_add(BigInt* a, BigInt* b)
{
    int is_negative = 0;
    if (a->msd >= 0 && b->msd >= 0)
    {
        is_negative = 0;
    }else if (a->msd < 0 && b->msd < 0)
    {
        is_negative = 1;
        a->msd = -a->msd;
        b->msd = -b->msd;

    }else if (a->msd > 0 && b->msd < 0)
    {
        b->msd = -b->msd;
        bigint_sub(a, b);
        b->msd = -b->msd;
        return;
    }else if (a->msd < 0 && b->msd > 0) 
    {
        a->msd = -a->msd;
        BigInt tmp = *b;
        bigint_sub(&tmp, a);
        *a = tmp;                                         
        return;
    }

    unsigned int max_len = max(a->digits[0], b->digits[0]);

    unsigned int original_a_len = a->digits[0];

    unsigned int *tmp = realloc(a->digits, sizeof(unsigned int) * (max_len + 2));
    if (tmp == NULL) return;
    a->digits = tmp;

    unsigned int carry = 0;
    unsigned long long digit_sum = 0;

    for (size_t i = 1; i <= max_len + 1; i++) //  from 1 to msd
    {
        unsigned int digit_a = bigint_get_digit(a, original_a_len, i);
        unsigned int digit_b = bigint_get_digit(b, b->digits[0], i);
        
        digit_sum = digit_a + digit_b + carry;
        carry = 0;
        if (digit_sum >= BASE)
        {
            digit_sum %= BASE;
            carry = 1;
        }
        a->digits[i] = digit_sum;
        
    }
    a->msd = (int)a->digits[max_len + 1];
    a->digits[0] = max_len;

    if (carry)
    {
        a->digits[max_len + 1] = (unsigned int)a->msd;
        a->msd = 1;
        a->digits[0] = max_len + 1;
    }

    if (is_negative)
    {
        a->msd = -a->msd;
        b->msd = -b->msd;
    }
}

void bigint_print(BigInt* a)
{   
    if (a->msd == 0)
    {
        printf("0");
        return;
    }
    
    printf("%d", a->msd);
    for (size_t i = a->digits[0]; i >= 1; i--)
    {
        printf("%d", a->digits[i]);
    }
}

void test_add(int a_val, int b_val, int expected)
{
    BigInt a, b;
    bigint_init(&a);
    bigint_init(&b);
    bigint_from_int(&a, a_val);
    bigint_from_int(&b, b_val);
    bigint_add(&a, &b);
    int result = a.msd;
    printf("%s: %d + %d = %d (expected %d)\n",
        result == expected ? "OK" : "FAIL",
        a_val, b_val, result, expected);
    bigint_free(&a);
    bigint_free(&b);
}

int main(void)
{
    test_add(3, 4, 7);
    test_add(0, 5, 5);
    test_add(5, 0, 5);
    test_add(0, 0, 0);
    test_add(100, 23, 123);
    test_add(-5, -3, -8);       // both negative
    // test_add(5, -3, 2);      // needs bigint_sub
    // test_add(-5, 3, -2);     // needs bigint_sub
    return 0;
}