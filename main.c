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
    num->digits[0] = 0;
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

unsigned int is_safe(BigInt *num)
{
    if (num == NULL || num->digits == NULL)
    {
        return 0;
    }
    return 1;
}

unsigned int bigint_get_digit(BigInt *num, unsigned int original_len, size_t i)
{
    if (!is_safe(num))
    {
        return 0;
    }
    
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

int bigint_compare(BigInt* a, BigInt* b)
{
    if (!is_safe(a) || !is_safe(b)) return 0;

    if (a->msd != b->msd)
        return (a->msd > b->msd) ? 1 : -1;

    int sign = (a->msd >= 0) ? 1 : -1;

    if (a->digits[0] != b->digits[0])
        return (a->digits[0] > b->digits[0]) ? sign : -sign;

    for (unsigned int i = a->digits[0]; i >= 1; i--)
    {
        if (a->digits[i] != b->digits[i])
            return (a->digits[i] > b->digits[i]) ? sign : -sign;
    }
    return 0;
}

BigInt bigint_copy(BigInt* src)
{
    BigInt copy;
    copy.msd = src->msd;
    copy.digits = malloc(sizeof(unsigned int) * (src->digits[0] + 2));
    for (unsigned int i = 0; i <= src->digits[0]; i++)
    {
        copy.digits[i] = src->digits[i];
    }
    return copy;
}

void bigint_add(BigInt* a, BigInt* b);

void bigint_sub(BigInt* a, BigInt* b)
{
    if (!is_safe(a) || !is_safe(b)) return;

    if (a->msd >= 0 && b->msd < 0)
    {
        b->msd = -b->msd;
        bigint_add(a, b);
        b->msd = -b->msd;
        return;
    }else if (a->msd < 0 && b->msd >= 0)
    {
        a->msd = -a->msd;
        bigint_add(a, b);
        a->msd = -a->msd;
        return;
    }else if (a->msd < 0 && b->msd < 0)
    {
        a->msd = -a->msd;
        b->msd = -b->msd;
        BigInt tmp = bigint_copy(b);
        bigint_sub(&tmp, a);
        free(a->digits);
        *a = tmp;
        b->msd = -b->msd;
        return;
    }

    unsigned int max_len = max(a->digits[0], b->digits[0]);
    unsigned int original_a_len = a->digits[0];

    unsigned int *tmp = realloc(a->digits, sizeof(unsigned int) * (max_len + 2));
    if (tmp == NULL) return;
    a->digits = tmp;

    unsigned int borrow = 0;

    for (size_t i = 1; i <= max_len + 1; i++)
    {
        unsigned int digit_a = bigint_get_digit(a, original_a_len, i);
        unsigned int digit_b = bigint_get_digit(b, b->digits[0], i);

        long long diff = (long long)digit_a - digit_b - borrow;
        borrow = 0;
        if (diff < 0)
        {
            diff += BASE;
            borrow = 1;
        }
        a->digits[i] = (unsigned int)diff;
    }

    a->msd = (int)a->digits[max_len + 1];
    a->digits[0] = max_len;

    while (a->digits[0] > 0 && a->msd == 0)
    {
        if ((int)a->digits[a->digits[0]] < 0) break;
        a->msd = (int)a->digits[a->digits[0]];
        a->digits[0]--;
    }
    if (a->digits[0] == 0) a->digits[0] = 1;
}

void bigint_add(BigInt* a, BigInt* b)
{
    if (!is_safe(a) || !is_safe(b)) return;

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
        BigInt tmp = bigint_copy(b);
        bigint_sub(&tmp, a);
        free(a->digits);
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
        
        digit_sum = (unsigned long long)digit_a + digit_b + carry;
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

void bigint_mul(BigInt* a, BigInt* b)
{
    if (!is_safe(a) || !is_safe(b)) return;

    int sign = 1;
    int saved_b_msd = b->msd;
    if (a->msd < 0) { sign = -sign; a->msd = -a->msd; }
    if (b->msd < 0) { sign = -sign; b->msd = -b->msd; }

    unsigned int a_len = a->digits[0] + 1;
    unsigned int b_len = b->digits[0] + 1;
    unsigned int res_size = a_len + b_len;

    unsigned int *res = calloc(res_size + 2, sizeof(unsigned int));
    if (res == NULL) return;

    for (unsigned int j = 1; j <= b_len; j++)
    {
        unsigned int bj = bigint_get_digit(b, b->digits[0], j);
        unsigned int carry = 0;

        for (unsigned int i = 1; i <= a_len; i++)
        {
            unsigned int ai = bigint_get_digit(a, a->digits[0], i);
            unsigned long long prod = (unsigned long long)ai * bj + res[i + j - 1] + carry;
            res[i + j - 1] = (unsigned int)(prod % BASE);
            carry = (unsigned int)(prod / BASE);
        }
        res[a_len + j] += carry;
    }

    unsigned int top = res_size;
    while (top > 1 && res[top] == 0)
        top--;

    free(a->digits);
    a->digits = res;
    a->msd = (int)res[top];
    a->digits[0] = top - 1;
    if (a->digits[0] == 0) a->digits[0] = 1;

    while (a->digits[0] > 0 && a->msd == 0)
    {
        if ((int)a->digits[a->digits[0]] < 0) break;
        a->msd = (int)a->digits[a->digits[0]];
        a->digits[0]--;
    }
    if (a->digits[0] == 0) a->digits[0] = 1;

    if (sign < 0)
        a->msd = -a->msd;
    b->msd = saved_b_msd;
}

void bigint_print(BigInt* a)
{
    if (!is_safe(a)) return;

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

void test_sub(int a_val, int b_val, int expected)
{
    BigInt a, b;
    bigint_init(&a);
    bigint_init(&b);
    bigint_from_int(&a, a_val);
    bigint_from_int(&b, b_val);
    bigint_sub(&a, &b);
    int result = a.msd;
    printf("%s: %d - %d = %d (expected %d)\n",
        result == expected ? "OK" : "FAIL",
        a_val, b_val, result, expected);
    bigint_free(&a);
    bigint_free(&b);
}

void test_add_big(int a_msd, unsigned int a_d1,
                  int b_msd, unsigned int b_d1,
                  int exp_msd, unsigned int exp_d1)
{
    BigInt a, b;
    bigint_init(&a);
    bigint_init(&b);

    unsigned int *tmp_a = realloc(a.digits, sizeof(unsigned int) * 3);
    if (tmp_a == NULL) return;
    a.digits = tmp_a;
    a.digits[0] = 1;
    a.digits[1] = a_d1;
    a.msd = a_msd;

    unsigned int *tmp_b = realloc(b.digits, sizeof(unsigned int) * 3);
    if (tmp_b == NULL) return;
    b.digits = tmp_b;
    b.digits[0] = 1;
    b.digits[1] = b_d1;
    b.msd = b_msd;

    bigint_add(&a, &b);
    int ok = (a.msd == exp_msd && a.digits[1] == exp_d1);
    printf("%s: [%d,%u] + [%d,%u] = [%d,%u] (expected [%d,%u])\n",
        ok ? "OK" : "FAIL",
        a_msd, a_d1, b_msd, b_d1,
        a.msd, a.digits[1],
        exp_msd, exp_d1);
    bigint_free(&a);
    bigint_free(&b);
}

void test_sub_big(int a_msd, unsigned int a_d1,
                  int b_msd, unsigned int b_d1,
                  int exp_msd, unsigned int exp_d1)
{
    BigInt a, b;
    bigint_init(&a);
    bigint_init(&b);

    unsigned int *tmp_a = realloc(a.digits, sizeof(unsigned int) * 3);
    if (tmp_a == NULL) return;
    a.digits = tmp_a;
    a.digits[0] = 1;
    a.digits[1] = a_d1;
    a.msd = a_msd;

    unsigned int *tmp_b = realloc(b.digits, sizeof(unsigned int) * 3);
    if (tmp_b == NULL) return;
    b.digits = tmp_b;
    b.digits[0] = 1;
    b.digits[1] = b_d1;
    b.msd = b_msd;

    bigint_sub(&a, &b);
    int ok = (a.msd == exp_msd && a.digits[1] == exp_d1);
    printf("%s: [%d,%u] - [%d,%u] = [%d,%u] (expected [%d,%u])\n",
        ok ? "OK" : "FAIL",
        a_msd, a_d1, b_msd, b_d1,
        a.msd, a.digits[1],
        exp_msd, exp_d1);
    bigint_free(&a);
    bigint_free(&b);
}

void test_mul(int a_val, int b_val, int expected)
{
    BigInt a, b;
    bigint_init(&a);
    bigint_init(&b);
    bigint_from_int(&a, a_val);
    bigint_from_int(&b, b_val);
    bigint_mul(&a, &b);
    int result = a.msd;
    printf("%s: %d * %d = %d (expected %d)\n",
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
    test_add(-5, -3, -8);
    test_add(5, -3, 2);
    test_add(-5, 3, -2);

    test_sub(10, 3, 7);
    test_sub(3, 10, -7);
    test_sub(0, 5, -5);
    test_sub(5, 0, 5);
    test_sub(0, 0, 0);
    test_sub(100, 23, 77);
    test_sub(-5, -3, -2);
    test_sub(-3, -5, 2);
    test_sub(5, -3, 8);
    test_sub(-5, 3, -8);

    test_add_big(1, 0, 1, 0, 2, 0);
    test_add_big(1, 5, 1, 10, 2, 15);
    test_add_big(1, UINT_MAX, 0, 1, 2, 0);
    test_sub_big(3, 100, 1, 50, 2, 50);
    test_sub_big(2, 0, 1, 1, 0, UINT_MAX);
    test_sub_big(1, 0, 1, 0, 0, 0);

    test_mul(3, 4, 12);
    test_mul(0, 5, 0);
    test_mul(5, 0, 0);
    test_mul(0, 0, 0);
    test_mul(100, 23, 2300);
    test_mul(-5, 3, -15);
    test_mul(5, -3, -15);
    test_mul(-5, -3, 15);
    test_mul(1, 1000000, 1000000);
    test_mul(-1, 1, -1);

    return 0;
}