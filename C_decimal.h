#ifndef DECIMAL_H
#define DECIMAL_H

#include <stdbool.h>

typedef struct
{
    int bits[4];    // bits[0] bits[1] bits[2] bits[3] 
} Decimal;

// Структура выполняет роль "широкого" decimal для работы и хранения промежуточного результата операций
typedef struct
{
    int bits[8];    // bits[0] bits[1] bits[2] bits[3] bits[4] bits[5] bits[6] bits[7]
} Long_decimal;

#define PARTS_OF_DECIMAL 4
#define PARTS_OF_LONG_DECIMAL 8
#define MIN_EXPONENT 0
#define MAX_EXPONENT 28
#define MAX_BITS_IN_MANTISSA_LONG_DECIMAL 224
#define BINARY_BASE 2
#define SIGN_PLUS 0
#define SIGN_MINUS 1
#define OK 0
#define NUMBER_IS_TOO_LARGE_OR_EQUAL_INFINITY 1
#define NUMBER_IS_TOO_SMALL_OR_EQUAL_NEGATIVE_INFINITY 2
#define DIVISION_BY_ZERO 3
#define ERROR_CONVERSION 1
#define LESS -1
#define EQUAL 0
#define GREATER 1

// Арифметические операторы (+ - * /)
int add(Decimal value_1, Decimal value_2, Decimal * result);
int sub(Decimal value_1, Decimal value_2, Decimal * result);
int mul(Decimal value_1, Decimal value_2, Decimal * result);
int divide(Decimal value_1, Decimal value_2, Decimal * result);

// Операторы сравнение (< <= > >= == !=)
int is_less(Decimal value_1, Decimal value_2);             
int is_less_or_equal(Decimal value_1, Decimal value_2);     
int is_greater(Decimal value_1, Decimal value_2);           
int is_greater_or_equal(Decimal value_1, Decimal value_2);  
int is_equal(Decimal value_1, Decimal value_2);             
int is_not_equal(Decimal value_1, Decimal value_2);        

// Преобразователи
int from_int_to_decimal(int src, Decimal * dst);
int from_float_to_decimal(float src, Decimal * dst);
int from_decimal_to_int(Decimal src, int * dst);
int from_decimal_to_float(Decimal src, float  * dst);

// Другие функции
int my_floor(Decimal value, Decimal * result);
int my_round(Decimal value, Decimal * result);
int truncate(Decimal value, Decimal * result);
int negate(Decimal value, Decimal * result);

// Вывод значений типов decimal и Long_decimal в десятичной системе счисления
void output_decimal_in_dec(Decimal val);
void output_Long_decimal_in_dec(Long_decimal val);

// Вывод значений типов decimal и Long_decimal в двоичной системе счисления
void output_decimal(const Decimal * value);
void output_Long_decimal(const Long_decimal * value);

/**************************************************************************************************************************/
// Вспомогательные функции
static int convert_Long_decimal_to_decimal(const Long_decimal * src, Decimal * dst);
static Long_decimal convert_decimal_to_Long_decimal(const Decimal * value);
static void clear_Long_decimal(Long_decimal * value);
static void clear_decimal(Decimal * value);
static void add_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result);
static void sub_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result);
static void multiply_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result);
static void division_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result);
static int align_dividend_and_divider(const Long_decimal * dividend, Long_decimal * divider);
static void shift_Long_decimal_to_left(Long_decimal * value, int shift);
static void shift_Long_decimal_to_right(Long_decimal * value, int shift);
static void mult_Long_decimal_on_10(Long_decimal * value);
static Long_decimal div_Long_decimal_on_10(Long_decimal * value);
static const Long_decimal * get_less_module(const Long_decimal * value_1, const Long_decimal * value_2);
static void invert_mantissa_Long_decimal(Long_decimal * value);
static int compare_decimal(Decimal * value_1, Decimal * value_2);
static int compare_Long_decimal(Long_decimal value_1, Long_decimal value_2);
static void make_equal_exponents(Long_decimal * val_1, Long_decimal * val_2);
static void convert_to_up_exponent(Long_decimal * value, int need_exponent);
static Long_decimal convert_to_down_exponent(Long_decimal * value);
static void bank_rounding(Long_decimal * value, const Long_decimal * fractional);
static void increment_mantissa_Long_decimal(Long_decimal * value);
static bool is_zero_Long_decimal(const Long_decimal * value);
static bool is_zero_decimal(const Decimal * value);
static Long_decimal discard_zeroes(Long_decimal value);
static bool valid_value(float value);
static bool is_float_zero(float value);
static bool is_negative_zero(float value);

// Функции для работы со знаком числа
static int get_sign_decimal(const Decimal * value);
static int get_sign_Long_decimal(const Long_decimal * value);
static void set_sign_decimal(Decimal * value, int sign);
static void set_sign_Long_decimal(Long_decimal * value, int sign);

// Функции для работы с экспонентой числа
static int get_exponent_decimal(const Decimal * value);
static int get_exponent_Long_decimal(const Long_decimal * value);
static void set_exponent_decimal(Decimal * value, int exponent);
static void set_exponent_Long_decimal(Long_decimal * value, int exponent);

// Функции для работы с мантиссой числа
static int get_bit_Long_decimal(const Long_decimal * value, int bit_number);
static void set_bit_Long_decimal(Long_decimal * value, int bit_number, int bit);
static int identify_part_of_mantissa(int bit_number);

#endif // DECIMAL_H
