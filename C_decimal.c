#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "C_decimal.h"

// Арифметические операторы (+ - * /)

int add(Decimal value_1, Decimal value_2, Decimal * result)
{
    int ret_val = OK;
    if(is_zero_decimal(&value_1) && is_zero_decimal(&value_2)) // если оба числа 0(+- и ^n), то и результат будет 0(+- и ^n)
    {
        clear_decimal(result);
        int sign_val_1 = get_sign_decimal(&value_1);
        int sign_val_2 = get_sign_decimal(&value_2);
        int exp_val_1 = get_exponent_decimal(&value_1);
        int exp_val_2 = get_exponent_decimal(&value_2);
        set_sign_decimal(result, sign_val_1 + sign_val_2 != SIGN_MINUS ? SIGN_PLUS : SIGN_MINUS);
        set_exponent_decimal(result, exp_val_1 > exp_val_2 ? exp_val_1 : exp_val_2);
    }
    else if(is_zero_decimal(&value_1) || is_zero_decimal(&value_2)) // если одно число == 0, то сумма будет равна другому числу
        *result = is_zero_decimal(&value_1) ? value_2 : value_1; 
    else // если ни одно из чисел != 0
    {
        Long_decimal val_1 = convert_decimal_to_Long_decimal(&value_1);
        Long_decimal val_2 = convert_decimal_to_Long_decimal(&value_2);
        Long_decimal res;
        if(get_sign_Long_decimal(&val_1) == get_sign_Long_decimal(&val_2)) // если у чисел при сложении одинаковые знаки,
            add_long(val_1, val_2, &res);  // то это действительно сложение
        else // если у чисел при сложении разные знаки,
            sub_long(val_1, val_2, &res);  // то это на самом деле вычитание
        ret_val = convert_Long_decimal_to_decimal(&res, result);
    }
    return ret_val;
}

int sub(Decimal value_1, Decimal value_2, Decimal * result)
{
    int ret_val = OK;
    if(is_zero_decimal(&value_1) && is_zero_decimal(&value_2))
    {
        if(get_sign_decimal(&value_2) == SIGN_MINUS)
            set_sign_decimal(&value_2, SIGN_PLUS);
    }
    else // меняем операцию вычитания на операцию сложения с изменением знака вычитаемого: 5 - 3 => 5 + (-3)
        set_sign_decimal(&value_2, (get_sign_decimal(&value_2) == SIGN_PLUS) ? SIGN_MINUS : SIGN_PLUS); // поменяли знак вычитаемого
    ret_val = add(value_1, value_2, result); // заменили sub() на add()
    return ret_val;
}

int mul(Decimal value_1, Decimal value_2, Decimal * result)
{
    int ret_val = OK;
    // любое число умноженное на 0 дает в результате 0
    if(is_zero_decimal(&value_1) || is_zero_decimal(&value_2))
    {
        clear_decimal(result);
        int sign_val_1 = get_sign_decimal(&value_1);
        int sign_val_2 = get_sign_decimal(&value_2);
        int exp_val_1 = get_exponent_decimal(&value_1);
        int exp_val_2 = get_exponent_decimal(&value_2);
        set_sign_decimal(result, sign_val_1 + sign_val_2 != SIGN_MINUS ? SIGN_PLUS : SIGN_MINUS);
        set_exponent_decimal(result, exp_val_1 + exp_val_2);
    }
    else
    {
        Long_decimal val_1 = convert_decimal_to_Long_decimal(&value_1);
        Long_decimal val_2 = convert_decimal_to_Long_decimal(&value_2);
        Long_decimal res;
        multiply_long(val_1, val_2, &res);
        ret_val = convert_Long_decimal_to_decimal(&res, result);
    }
    return ret_val;
}

int divide(Decimal value_1, Decimal value_2, Decimal * result)
{
    int ret_val = OK;
    if(is_zero_decimal(&value_1) || is_zero_decimal(&value_2)) 
    {
        if(is_zero_decimal(&value_2)) // деление на 0 недопустимо
            ret_val = DIVISION_BY_ZERO;
        else // ноль разделить на любое число == 0
            clear_decimal(result);
    }
    else
    {
        Long_decimal val_1 = convert_decimal_to_Long_decimal(&value_1);
        Long_decimal val_2 = convert_decimal_to_Long_decimal(&value_2);
        Long_decimal res;
        division_long(val_1, val_2, &res);
        ret_val = convert_Long_decimal_to_decimal(&res, result);
    }
    return ret_val; 
}

// Операторы сравнение (< <= > >= == !=)

int is_less(Decimal value_1, Decimal value_2) 
{
    return compare_decimal(&value_1, &value_2) == LESS ? true : false;
}

int is_less_or_equal(Decimal value_1, Decimal value_2) 
{
    return compare_decimal(&value_1, &value_2) != GREATER ? true : false;
}

int is_greater(Decimal value_1, Decimal value_2) 
{   
    return compare_decimal(&value_1, &value_2) == GREATER ? true : false;
}

int is_greater_or_equal(Decimal value_1, Decimal value_2) 
{
    return compare_decimal(&value_1, &value_2) != LESS ? true : false;
}

int is_equal(Decimal value_1, Decimal value_2) 
{
    return compare_decimal(&value_1, &value_2) == EQUAL ? true : false;
}

int is_not_equal(Decimal value_1, Decimal value_2) 
{   
    return compare_decimal(&value_1, &value_2) != EQUAL ? true : false;
}

// Преобразователи

int from_int_to_decimal(int src, Decimal * dst)
{
    int ret_val = OK; 
    clear_decimal(dst);
    if(src < 0) // если число отрицательное
    {
        set_sign_decimal(dst, SIGN_MINUS); // значит и результат отрицательный
        src *= -1;                         // делаем исходный int положительным
    }
    dst->bits[0] = src;
    return ret_val;
}

int from_float_to_decimal(float src, Decimal * dst)
{
    int ret_val = OK;
    char str_float[16] = { 0 };
    int integer = 0;
    int exponent = 0;
    int i = 0;

    if(valid_value(src))
    {
        if(is_float_zero(src))
        {
            clear_decimal(dst);
            if(is_negative_zero(src))
                set_sign_decimal(dst, SIGN_MINUS);
        }
        // если (|src| > 79,228,162,514,264,337,593,543,950,335), т.е слишком большое
        else if((unsigned long)(fabs(fabs(src) - 79228162514264330000000000000.0)) >= 8796093022208ul)
            ret_val = ERROR_CONVERSION;
        else
        {
            sprintf(str_float, "%.6e", src);               // преобразуем float в строковое представление
            while(str_float[i] != '.')                     // ищем точку
                ++i;
            for(int j = 1; j <= 6; ++j, ++i)               // делаем строковое число целым перенося точку вправо
            {
                char temp = str_float[i + 1];
                str_float[i + 1] = str_float[i];
                str_float[i] = temp;
            }
            str_float[i] = '\0';                           // делим строковое число на число и экспоненту "1e2" => "1\02"
            sscanf(str_float, "%d", &integer);             // получаем целое число
            sscanf(&str_float[i + 2], "%d", &exponent);    // получаем экспоненту

            // если (|src| > 79,228,162,514,264,337,593,543,950,335), т.е слишком большое
            if(exponent > MAX_EXPONENT || (exponent == MAX_EXPONENT && integer > 7922816))   
                ret_val = ERROR_CONVERSION; 
            else if(exponent < -MAX_EXPONENT)              // если (0 < |src| < 1e-28), т.е. слишком маленькое, но != 0
            {
                ret_val = ERROR_CONVERSION;
                clear_decimal(dst);
            }
            else // если число входит в диапазон
            {
                exponent -= 6;                             // поскольку делали число целым, то нужно вычислить исходное место точки
                from_int_to_decimal(integer, dst);
                Long_decimal temp_dst = convert_decimal_to_Long_decimal(dst);
                if(exponent > 0)                           // если число целое (т.е. без экспоненты)
                {
                    for(int ctr = 0; ctr < exponent; ++ctr)
                        mult_Long_decimal_on_10(&temp_dst);
                }
                else                                       // если число дробное (т.е. c экспонентой), включая те, которые могут 
                {                                          // быть преобразованы к целому, путем отбрасывания дробной части == 0
                    exponent *= -1;
                    set_exponent_Long_decimal(&temp_dst, exponent);
                }
                temp_dst = discard_zeroes(temp_dst);
                convert_Long_decimal_to_decimal(&temp_dst, dst);
            }
        }
    }
    else
        ret_val = ERROR_CONVERSION;
    return ret_val;
}

int from_decimal_to_int(Decimal src, int * dst)
{
    int ret_val = OK;
    if(is_zero_decimal(&src))
        *dst = 0;
    else
    {
        truncate(src, &src);                          // получаем целую часть
        if(src.bits[2] == 0 && src.bits[1] == 0)      // если целое число входит в диапазон одного int
        {
            *dst = src.bits[0];
            if(get_sign_decimal(&src) == SIGN_MINUS)  // если исходный Decimal отрицательный, то и int тоже отрицательный
                *dst *= -1;                           // делаем результат отрицательным
        }
        else
            ret_val = ERROR_CONVERSION; // число выходит за диапазон типа int
    }
    return ret_val;
}

int from_decimal_to_float(Decimal src, float  * dst)
{
    int ret_val = OK;
    enum {LAST_CHAR = 29, SIZE};
    char  prev_str_float[SIZE] = { 0 };
    char final_str_float[SIZE] = { 0 };
    
    Long_decimal temp_src = convert_decimal_to_Long_decimal(&src);
    temp_src = discard_zeroes(temp_src);
    int prev_exponent = get_exponent_Long_decimal(&temp_src);
    set_exponent_Long_decimal(&temp_src, MIN_EXPONENT);
    set_sign_Long_decimal(&temp_src, SIGN_PLUS);
    int exponent = 0;
    int cur_char = LAST_CHAR;

    while(!is_zero_Long_decimal(&temp_src))
    {
        Long_decimal temp_fractional = div_Long_decimal_on_10(&temp_src);
        prev_str_float[cur_char--] = temp_fractional.bits[0] + '0';
        ++exponent;
    }
    if(exponent > prev_exponent)  // если целая часть числа > 0
    {
        prev_str_float[cur_char--] = '.';
        prev_str_float[cur_char--] = temp_src.bits[0] + '0';
        strcpy(final_str_float, &prev_str_float[cur_char] + 1);
        int temp_exponent = exponent - prev_exponent;
        cur_char = LAST_CHAR;
        while(temp_exponent != 0)
        {
            prev_str_float[cur_char--] = (temp_exponent % 10) + '0';
            temp_exponent /= 10;
        }
        strcat(final_str_float, "e");
        strcat(final_str_float, &prev_str_float[cur_char] + 1);
    }
    else   // если целая часть числа == 0
    {
        for(int i = prev_exponent - exponent; i > 0; --i)
            prev_str_float[cur_char--] = '0';
        prev_str_float[cur_char--] = '.';
        prev_str_float[cur_char--] = temp_src.bits[0] + '0';
        strcat(final_str_float, &prev_str_float[cur_char] + 1);
    }
    sscanf(final_str_float, "%f", dst);
    if(get_sign_decimal(&src) == SIGN_MINUS)
        *dst *= -1;  
    return ret_val;
}

// Другие функции

int my_floor(Decimal value, Decimal * result)
{ // Функция округляет указанное Decimal число до ближайшего целого числа в сторону отрицательной бесконечности
  // Если число отрицательно то округляем в сторону отрицательной бесконечности, т.е. из -2.5 -> -3, но
  // если положительно, то просто отбрасываем дробную часть, т.е. из 2.5 -> 2
    int ret_val = OK;  
    int exponent = get_exponent_decimal(&value);
    if(exponent != MIN_EXPONENT) // если есть дробная часть по которой программа будет округлять
    {
        Long_decimal temp_val = convert_decimal_to_Long_decimal(&value);
        Long_decimal integer_part    = temp_val;
        Long_decimal fractional_part = temp_val;
        // получаем целую часть
        while(exponent != MIN_EXPONENT)              
        {
            convert_to_down_exponent(&integer_part);
            --exponent;
        }
        // получаем дробную часть
        sub_long(temp_val, integer_part, &fractional_part);
        // округляем      
        if(!is_zero_Long_decimal(&fractional_part) && get_sign_Long_decimal(&integer_part) == SIGN_MINUS)                                               
            increment_mantissa_Long_decimal(&integer_part);
        ret_val = convert_Long_decimal_to_decimal(&integer_part, result);
    }
    else // если нет дробной части, то и округлять нечего
        *result = value;
    return ret_val;
}

int my_round(Decimal value, Decimal * result)
{ //  Функция округляет Decimal до ближайшего целого числа 
  // (в библиотеке math.h my_round() использует математическое округление, поэтому оно здесь и реализовано)
    int ret_val = OK;  
    int exponent = get_exponent_decimal(&value);
    if(exponent != MIN_EXPONENT) // если есть дробная часть по которой программа будет округлять
    {
        Long_decimal temp_val = convert_decimal_to_Long_decimal(&value);
        Long_decimal integer_part    = temp_val;
        Long_decimal fractional_part = temp_val;
        Long_decimal five = {{ 5, 0, 0, 0, 0, 0, 0, 0 }};
        set_exponent_Long_decimal(&five, 1); // получаем 0.5
        // получаем целую часть
        while(exponent != MIN_EXPONENT)              
        {
            convert_to_down_exponent(&integer_part);
            --exponent;
        }
        // получаем дробную часть с положительным знаком
        sub_long(temp_val, integer_part, &fractional_part);
        set_sign_Long_decimal(&fractional_part, SIGN_PLUS);
        // округляем      
        make_equal_exponents(&five, &fractional_part);
        if(compare_Long_decimal(fractional_part, five) != LESS) // если fractional_part >= five
            increment_mantissa_Long_decimal(&integer_part);
        ret_val = convert_Long_decimal_to_decimal(&integer_part, result);
    }
    else // если нет дробной части, то и округлять нечего
        *result = value;
    return ret_val;
}

int truncate(Decimal value, Decimal * result)
{//  Функция возвращает целые цифры указанного Decimal числа; любые дробные цифры отбрасываются, включая конечные нули
    int ret_val = OK;  
    int exponent = get_exponent_decimal(&value);
    if(exponent != MIN_EXPONENT) // если есть дробная часть по которой программа будет округлять
    {
        Long_decimal integer_part = convert_decimal_to_Long_decimal(&value);
        // получаем целую часть
        while(exponent != MIN_EXPONENT)              
        {
            convert_to_down_exponent(&integer_part);
            --exponent;
        }
        integer_part = discard_zeroes(integer_part);
        ret_val = convert_Long_decimal_to_decimal(&integer_part, result);
    }
    else // если нет дробной части, то и округлять нечего
        *result = value;
    return ret_val;
}

int negate(Decimal value, Decimal * result)
{
    int ret_val = OK; 
    // меняем знак на противоположный
    set_sign_decimal(&value, (get_sign_decimal(&value) == SIGN_PLUS ? SIGN_MINUS : SIGN_PLUS));
    *result = value;
    return ret_val;
}

// Вывод значений типов decimal и Long_decimal в десятичной системе счисления

void output_decimal_in_dec(Decimal val)
{
    Long_decimal temp_val = convert_decimal_to_Long_decimal(&val);
    output_Long_decimal_in_dec(temp_val);
}

void output_Long_decimal_in_dec(Long_decimal val)
{
    char str[150];
    int index = 0;
    int characters = 0;
    int exponent = get_exponent_Long_decimal(&val);
    set_exponent_Long_decimal(&val, MIN_EXPONENT);
    int sign = get_sign_Long_decimal(&val);
    set_sign_Long_decimal(&val, SIGN_PLUS);
    while(!is_zero_Long_decimal(&val))
    {
        ++characters;
        if(index == exponent)
            str[index++] = '.';
        Long_decimal temp_fractional = div_Long_decimal_on_10(&val);
        str[index] = temp_fractional.bits[0] + '0';
        ++index;
    }
    if(exponent >= characters)
    {
        for(int ctr = exponent - characters; ctr > 0; --ctr)
            str[index++] = '0'; 
        str[index++] = '.';
        str[index++] = '0';
    }
    if(sign == SIGN_MINUS)
        str[index++] = '-';
    str[index--] = '\0';
    while(index >= 0)
    {
        if(index == 0 && str[index] == '.')
        {
            --index;
            continue;
        } 
        printf("%c", str[index--]);
    }
    printf("\n");
}

// Вывод значений типов decimal и Long_decimal в двоичной системе счисления

void output_decimal(const Decimal * value)
{
    // bits[3] 0XXXXXXX-ХХХ00000-XXXXXXXX-XXXXXXXX
    // bits[2] 00000000-00000000-00000000-00000000
    // bits[1] 00000000-00000000-00000000-00000000
    // bits[0] 00000000-00000000-00000000-00000000
    
    char buffer[36] = {[35] = '\0'};
    for(int i = PARTS_OF_DECIMAL - 1; i >= 0; i--)
    {
        unsigned int temp_num = value->bits[i];
        for(int j = 34; j >= 0; j--)
        {
            if(j == 26 || j == 17 || j == 8)
            {
                buffer[j] = '-';
                j--;
            }
            buffer[j] = temp_num % 2 + '0';
            temp_num /= 2;
        }
        printf("[%d] %s\n", i, buffer);
    }
    printf("\n");
}

void output_Long_decimal(const Long_decimal * value)
{
    // bits[7] 0XXXXXXX-ХХХ00000-XXXXXXXX-XXXXXXXX
    // bits[6] 00000000-00000000-00000000-00000000
    // bits[5] 00000000-00000000-00000000-00000000
    // bits[4] 00000000-00000000-00000000-00000000
    // bits[3] 00000000-00000000-00000000-00000000
    // bits[2] 00000000-00000000-00000000-00000000
    // bits[1] 00000000-00000000-00000000-00000000
    // bits[0] 00000000-00000000-00000000-00000000
    char buffer[36] = {[35] = '\0'};
    for(int i = PARTS_OF_LONG_DECIMAL - 1; i >= 0; i--)
    {
        unsigned int temp_num = value->bits[i];
        for(int j = 34; j >= 0; j--)
        {
            if(j == 26 || j == 17 || j == 8)
            {
                buffer[j] = '-';
                j--;
            }
            buffer[j] = temp_num % 2 + '0';
            temp_num /= 2;
        }
        printf("[%d] %s\n", i, buffer);
    }
    printf("\n");
}


/***********************************************************************************************************************************/

// Вспомогательные функции

static int convert_Long_decimal_to_decimal(const Long_decimal * src, Decimal * dst)
{
    int ret_val = OK;
    Long_decimal temp_value = *src;
    Long_decimal fractional; // возможный остаток от деления при попытке сделать число валидным
    clear_Long_decimal(&fractional);
    int exponent = get_exponent_Long_decimal(&temp_value);
    int exponent_fractional = 0;
    // если при умножении сумма экспонент больше MAX_EXPONENT либо после других операций в расширенной части установлены 
    // биты и экспонента != 0, то преобразуем число в число равное по значению, но с меньшей экспонентой, пока 
    // оно не станет валидным либо экспонента не станет == 0
    while((exponent > MAX_EXPONENT) || 
        ((temp_value.bits[6] || temp_value.bits[5] || temp_value.bits[4] || temp_value.bits[3]) && exponent != MIN_EXPONENT))
    {
        // часть преобразуемых значений не может быть преобразована с полной точностью, как следствие - после преобразования 
        // такого числа остается дробная часть, исходя из которой, в последствии будет произведено округление полученного числа
        Long_decimal temp_fractional = convert_to_down_exponent(&temp_value);
        make_equal_exponents(&fractional, &temp_fractional);
        add_long(fractional, temp_fractional, &fractional); // в случае нескольких преобразований, получаемые остатки суммируются
                                        // т.е. округление не по послeдней цифре числа, а по всем отброшенным, т.е. например 
                                        // не по 0.3, а по 0.325
        exponent = get_exponent_Long_decimal(&temp_value);
        ++exponent_fractional;
    }
    if(!is_zero_Long_decimal(&fractional))       // если есть дробная часть
    {
        set_exponent_Long_decimal(&fractional, exponent_fractional); // получаем дробную часть в виде 0.123, а не 0.000123
        bank_rounding(&temp_value, &fractional); // то используем банковское округление
    }
    if(temp_value.bits[6] == 0 && temp_value.bits[5] == 0 && 
       temp_value.bits[4] == 0 && temp_value.bits[3] == 0) // если удалось сделать число валидным
    {
        for(int part = 0; part < PARTS_OF_DECIMAL; ++part) // сохраняем мантиссу
            dst->bits[part] = temp_value.bits[part];
        dst->bits[3] = 0; // подготавливаем часть со знаком и экспонентой путем очищения этой части
        set_exponent_decimal(dst, get_exponent_Long_decimal(&temp_value));
        set_sign_decimal(dst, get_sign_Long_decimal(src));
    }
    else // безуспешное преобразование
    {
        ret_val = get_sign_Long_decimal(src) == SIGN_PLUS ? 
            NUMBER_IS_TOO_LARGE_OR_EQUAL_INFINITY : NUMBER_IS_TOO_SMALL_OR_EQUAL_NEGATIVE_INFINITY;
    }
    return ret_val;
}

static Long_decimal convert_decimal_to_Long_decimal(const Decimal * value)
{
    Long_decimal result;
    clear_Long_decimal(&result);
    if(!is_zero_decimal(value))
    {
        for(int part = 0; part < PARTS_OF_DECIMAL - 1; ++part) // копируем мантиссу
            result.bits[part] = value->bits[part];
    }
    set_exponent_Long_decimal(&result, get_exponent_decimal(value));
    set_sign_Long_decimal(&result, get_sign_decimal(value));
    return result;
}

static void clear_Long_decimal(Long_decimal * value)
{
    for(int i = 0; i < PARTS_OF_LONG_DECIMAL; i++)
        value->bits[i] = 0;
}

static void clear_decimal(Decimal * value)
{
    for(int i = 0; i < PARTS_OF_DECIMAL; i++)
        value->bits[i] = 0;
}

static void add_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result)
{ // Функция правильно складывает только если числа с одинаковыми знаками
    int transfer = 0;
    make_equal_exponents(&value_1, &value_2); // выравниваем десятичный разделитель
    clear_Long_decimal(result);
    for(int current_bit = 0; current_bit < MAX_BITS_IN_MANTISSA_LONG_DECIMAL; current_bit++)
    {
        int sum = get_bit_Long_decimal(&value_1, current_bit) + get_bit_Long_decimal(&value_2, current_bit) + transfer;
        transfer = sum / BINARY_BASE; // вычисляем, есть ли перенос в следующий разряд
        sum %= BINARY_BASE;           // получаем результат значения текущего разряда
        set_bit_Long_decimal(result, current_bit, sum);
    }
    set_exponent_Long_decimal(result, get_exponent_Long_decimal(&value_1));
    set_sign_Long_decimal(result, get_sign_Long_decimal(&value_1));
}

static void sub_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result)
{   // Функция правильно вычитает только если числа имеют равные экспоненты
    Long_decimal * less_module = (Long_decimal*)get_less_module(&value_1, &value_2); // определяем, какое число по модулю меньше   
    Long_decimal * greater_module = (less_module == &value_1) ? &value_2 : &value_1;
    int sign_greater = get_sign_Long_decimal(greater_module); // сохраняем знак для итогового результата
    set_sign_Long_decimal(less_module, SIGN_PLUS);    // получили модуль меньшего числа
    set_sign_Long_decimal(greater_module, SIGN_PLUS); // получили модуль большего числа
    invert_mantissa_Long_decimal(less_module);        // необходимо для вычитания
    increment_mantissa_Long_decimal(less_module);     // необходимо для вычитания
    add_long(*greater_module, *less_module, result);       // вычитание "через сложение"
    set_sign_Long_decimal(result, sign_greater);      // устанавливаем знак того числа, модуль которого больше
}

static void multiply_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result)
{
    // определяем знак результата. Если сумма знаков чисел дает 1(т.е. SIGN_MINUS), то и у результата будет минус.
    int sign = (get_sign_Long_decimal(&value_1) + get_sign_Long_decimal(&value_2) == SIGN_MINUS) ? SIGN_MINUS : SIGN_PLUS;
    // определяем экспоненту результата путем сложения исходных экспонент
    int exponent = get_exponent_Long_decimal(&value_1) + get_exponent_Long_decimal(&value_2);
    // получаем модули чисел
    set_sign_Long_decimal(&value_1, SIGN_PLUS);
    set_sign_Long_decimal(&value_2, SIGN_PLUS);
    // обнуляем экспоненты для правильного вычисления результата
    set_exponent_Long_decimal(&value_1, MIN_EXPONENT);
    set_exponent_Long_decimal(&value_2, MIN_EXPONENT);
    clear_Long_decimal(result); // очищаем для будущего результата
    // проходим по битам множителя
    for(int bit = MAX_BITS_IN_MANTISSA_LONG_DECIMAL - 1; bit >= 0; --bit)
    {
        Long_decimal temp_value_1 = value_1; // необходимо для правильного вычисления результата
        if(get_bit_Long_decimal(&value_2, bit) == 1)
        {
            shift_Long_decimal_to_left(&temp_value_1, bit); // сдвигаем биты temp_value_1 на bit влево
            add_long(*result, temp_value_1, result);
        }
    }
    set_exponent_Long_decimal(result, exponent);
    set_sign_Long_decimal(result, sign);
}

static void division_long(Long_decimal value_1, Long_decimal value_2, Long_decimal * result)
{
    Long_decimal temp_result; // хранит промежуточный результат
    clear_Long_decimal(&temp_result);
    int temp_exponent = get_exponent_Long_decimal(&value_1);
    int ctr_shifts = 0;
    int exponent = 0;
    clear_Long_decimal(result);
    // определяем знак результата. Если сумма знаков чисел дает 1(т.е. SIGN_MINUS), то и у результата будет минус.
    int sign = (get_sign_Long_decimal(&value_1) + get_sign_Long_decimal(&value_2) == SIGN_MINUS) ? SIGN_MINUS : SIGN_PLUS;
    set_sign_Long_decimal(&value_1, SIGN_PLUS); // необходимо для надлежащего вычитания
    set_sign_Long_decimal(&value_2, SIGN_PLUS); // необходимо для надлежащего вычитания
    make_equal_exponents(&value_1, &value_2);
    while(compare_Long_decimal(value_1, value_2) == LESS)
    {
        mult_Long_decimal_on_10(&value_1); // умножаем мантиссу value_1 на 10, чтобы делимое стало больше делителя
        ++exponent;                        // поскольку изначально делимое было меньше делителя, то результат будет < 1 
    }
    ctr_shifts = align_dividend_and_divider(&value_1, &value_2);
    while(!is_zero_Long_decimal(&value_1) && exponent <= MAX_EXPONENT + 1)
    {
        sub_long(value_1, value_2, &value_1);
        set_bit_Long_decimal(&temp_result, 0, 1); // устанавливаем бит в текущем частном
        if(is_zero_Long_decimal(&value_1))        // если больше нечего делить
        {
            while(ctr_shifts > 0)
            {
                shift_Long_decimal_to_left(&temp_result, 1);
                --ctr_shifts;
            }
        }
        else // если деление еще не окончено
        {
            while(compare_Long_decimal(value_1, value_2) == LESS && ctr_shifts > 0) // пытаемся сделать делимое больше делителя
            {
                shift_Long_decimal_to_left(&temp_result, 1);   // "собираем" временный результат
                shift_Long_decimal_to_right(&value_2, 1);      // "подгоняем" делитель под делимое
                --ctr_shifts;
            }
            while(compare_Long_decimal(value_1, value_2) == LESS) // если не удалось сделать делимое больше делителя, значит
            {                                                     // настало время получения дробной части из этого остатка
                ++exponent;
                mult_Long_decimal_on_10(&value_1);      // "дописываем ноль" к остатку от деления       
                mult_Long_decimal_on_10(&temp_result);  // необходимо для итогового результата
                mult_Long_decimal_on_10(result);        // необходимо для итогового результата
                add_long(*result, temp_result, result); // *result "собирает" итоговый результат
                clear_Long_decimal(&temp_result);       // очищаем для хранения очередного частного от деления остатка
                ctr_shifts = align_dividend_and_divider(&value_1, &value_2);
            }
        }
    }
    if(is_zero_Long_decimal(result)) 
    {
        set_exponent_Long_decimal(&temp_result, temp_exponent);
        while (temp_exponent-- > 0) 
            mult_Long_decimal_on_10(&temp_result);
        add_long(*result, temp_result, result);
    } 
    else 
    {
        add_long(*result, temp_result, result);
        set_exponent_Long_decimal(result, exponent);
    }
    set_sign_Long_decimal(result, sign);
}

static int align_dividend_and_divider(const Long_decimal * dividend, Long_decimal * divider)
{ // Функция "подгоняет" делитель под делимое и возвращает количество сдвигов делителя
    int ctr_shifts = 0;
    while(compare_Long_decimal(*dividend, *divider) == GREATER) 
    {
        shift_Long_decimal_to_left(divider, 1);
        ++ctr_shifts;
    }
    if(compare_Long_decimal(*dividend, *divider) == LESS && ctr_shifts > 0)
    {
        shift_Long_decimal_to_right(divider, 1);
        --ctr_shifts;
    }
    return ctr_shifts;
}

static void shift_Long_decimal_to_left(Long_decimal * value, int shift)
{   // Функция сдвигает биты *value влево на shift бит
    for(int ctr = 0; ctr < shift; ++ctr)
    {
        int prev_transfer = 0;
        for(int part = 0; part < PARTS_OF_LONG_DECIMAL - 1; ++part)
        {
            int transfer = (value->bits[part] >> 31) & 0x1;// сохранили старший бит в bits[part] для возможного переноса в bits[part + 1]
            value->bits[part] <<= 1;                    // сдвигаем биты bits[part] влево на 1 бит
            if(prev_transfer)                           // если от bits[part - 1] был перенос, то его учитываем в bits[part]
                value->bits[part] |= 0x1;
            prev_transfer = transfer;                   // transfer для bits[part] это prev_transfer для bits[part + 1]
        }
        //if(transfer) printf("OVERFLOW\n"); // - значит мы вышли даже за границы Long_decimal (но такого по сути не должно случиться)
    }
}

static void shift_Long_decimal_to_right(Long_decimal * value, int shift)
{   // Функция сдвигает биты *value вправо на shift бит
    for(int ctr = 0; ctr < shift; ++ctr)
    {
        int prev_transfer = 0;
        for(int part = PARTS_OF_LONG_DECIMAL - 2; part >= 0; --part)
        {
            int transfer = value->bits[part] & 0x1; // сохранили младший бит в bits[part] для возможного переноса в bits[part - 1]
            if(((value->bits[part] >> 31) & 0x1) == 1) // если старший бит в bits[part] == 1,
            {                                          // то при сдвиге вправо предыдущий бит тоже станет == 1. Это такая 
                value->bits[part] >>= 1;               // логика побитовых операций, поэтому если у нас такой случай, то мы 
                value->bits[part] &= 0x7FFFFFFF;       // сдвигаем биты и самый старший бит обнуляем для последующих сдвигов 
            }
            else
                value->bits[part] >>= 1;               // сдвигаем биты bits[part] вправо на 1 бит
            if(prev_transfer)                          // если от bits[part + 1] был перенос, то его учитываем в bits[part]
                value->bits[part] |= 0x80000000;       
            prev_transfer = transfer;                  // transfer для bits[part] это prev_transfer для bits[part - 1]
        }
    }
}

static const Long_decimal * get_less_module(const Long_decimal * value_1, const Long_decimal * value_2)
{
    Long_decimal copy_val_1 = *value_1;
    Long_decimal copy_val_2 = *value_2;
    // Получаем модули
    set_sign_Long_decimal(&copy_val_1, SIGN_PLUS);
    set_sign_Long_decimal(&copy_val_2, SIGN_PLUS);
    return (compare_Long_decimal(copy_val_1, copy_val_2) == LESS) ? value_1 : value_2;
}

static void invert_mantissa_Long_decimal(Long_decimal * value)
{
    for(int part = 0; part < PARTS_OF_LONG_DECIMAL - 1; ++part)
        value->bits[part] = ~value->bits[part]; // инвертирование битов в value->bits[part]
}

static int compare_decimal(Decimal * value_1, Decimal * value_2)
{   // Функция возвращает: -1 если value_1 < value_2; 0 если value_1 == value_2; 1 если value_1 > value_2
    int ret_val;

    int sign_val_1 = get_sign_decimal(value_1);
    int sign_val_2 = get_sign_decimal(value_2);

    if(is_zero_decimal(value_1) && is_zero_decimal(value_2))
        ret_val = EQUAL;
    else if(sign_val_1 == sign_val_2)
    {
        Long_decimal val_1 = convert_decimal_to_Long_decimal(value_1);
        Long_decimal val_2 = convert_decimal_to_Long_decimal(value_2);
        ret_val = compare_Long_decimal(val_1, val_2);
    }
    else
        ret_val = (sign_val_1 == SIGN_MINUS) ? LESS : GREATER;
    return ret_val;
}

static int compare_Long_decimal(Long_decimal value_1, Long_decimal value_2)
{   // Функция возвращает: -1 если value_1 < value_2; 0 если value_1 == value_2; 1 если value_1 > value_2
    int ret_val = EQUAL;
    
    int sign_val_1 = get_sign_Long_decimal(&value_1);
    int sign_val_2 = get_sign_Long_decimal(&value_2);

    if(sign_val_1 == sign_val_2)
    {
        bool values_equal = true;
        make_equal_exponents(&value_1, &value_2);
        for(int cur_bit = 0; cur_bit < MAX_BITS_IN_MANTISSA_LONG_DECIMAL; ++cur_bit) // проверка на равенство чисел
        {
            int bit_val_1 = get_bit_Long_decimal(&value_1, cur_bit);
            int bit_val_2 = get_bit_Long_decimal(&value_2, cur_bit);
            if(bit_val_1 != bit_val_2)
            {
                values_equal = false;
                break;
            }
        }
        if(!values_equal)
        {
            for(int cur_bit = MAX_BITS_IN_MANTISSA_LONG_DECIMAL - 1; cur_bit >= 0; --cur_bit)
            {
                int bit_val_1 = get_bit_Long_decimal(&value_1, cur_bit);
                int bit_val_2 = get_bit_Long_decimal(&value_2, cur_bit);
                if(bit_val_1 != bit_val_2)
                {
                    if(sign_val_1 == SIGN_PLUS)
                        ret_val = (bit_val_1 < bit_val_2) ? LESS : GREATER;
                    else // if(sign_val_1 == SIGN_MINUS)
                        ret_val = (bit_val_1 > bit_val_2) ? LESS : GREATER;
                    break;
                }
            }
        }
    }
    else
        ret_val = (sign_val_1 == SIGN_MINUS) ? LESS : GREATER;
    return ret_val;
}

static void make_equal_exponents(Long_decimal * value_1, Long_decimal * value_2)
{   // Функция, если это требуется, выравнивает в числах десятичный разделитель (точку) путем преобразования числа с 
    // меньшей экспонентой в равнозначное число с требуемой экспонентой, например: (0)3 = 3 -> (1)30 = 3.0 -> (2)300 = 3.00 
    int exp_val_1 = get_exponent_Long_decimal(value_1);
    int exp_val_2 = get_exponent_Long_decimal(value_2); 
    if(exp_val_1 != exp_val_2)
    {   
        if(exp_val_1 < exp_val_2)
            convert_to_up_exponent(value_1, exp_val_2);
        else
            convert_to_up_exponent(value_2, exp_val_1);
    }
}

static void convert_to_up_exponent(Long_decimal * value, int need_exponent)
{ // Функция преобразует число в число равное по значению, но с большей экспонентой
    for(int cur_exp = get_exponent_Long_decimal(value); cur_exp < need_exponent; ++cur_exp)
        mult_Long_decimal_on_10(value);              // умножаем мантиссу на 10
    set_exponent_Long_decimal(value, need_exponent); // устанавливаем требуемую экспоненту
}

static Long_decimal convert_to_down_exponent(Long_decimal * value)
{ // Функция делит мантиссу на 10 и декрементирует экспоненту, причем *value будет с 
  // правильными экспонентой и знаком, а остаток будет положительным и с MIN_EXPONENT
    Long_decimal fractional;
    int sign = get_sign_Long_decimal(value);          // сохраняем знак для целой части
    set_sign_Long_decimal(value, SIGN_PLUS);          // получаем модуль
    int exponent = get_exponent_Long_decimal(value);  // сохраняем экспоненту для целой части
    set_exponent_Long_decimal(value, MIN_EXPONENT);   // делаем целым для правильного деления на 10
    fractional = div_Long_decimal_on_10(value);
    set_exponent_Long_decimal(&fractional, exponent);
    set_sign_Long_decimal(value, sign);               // восстанавливаем знак для целой части
    set_exponent_Long_decimal(value, exponent - 1);   // уменьшаем исходную экспоненту на 1 в целой части
    return fractional;                               
}

static void mult_Long_decimal_on_10(Long_decimal * value)
{ // Функция умножает мантиссу и не изменяет экспоненту
    Long_decimal temp_1 = *value;
    Long_decimal temp_2 = *value;
    int exponent = get_exponent_Long_decimal(value);
    set_exponent_Long_decimal(value, MIN_EXPONENT);
    shift_Long_decimal_to_left(&temp_1, 1);
    shift_Long_decimal_to_left(&temp_2, 3);
    add_long(temp_1, temp_2, value);
    set_exponent_Long_decimal(value, exponent);
}

static Long_decimal div_Long_decimal_on_10(Long_decimal * value)
{   // Функция работает с целым числом и "возвращает" целую часть от деления через *value, а дробную - через return
    Long_decimal ten = { { 10, 0, 0, 0, 0, 0, 0, 0 } };
    Long_decimal temp_value_1 = *value;
    Long_decimal temp_value_2 = *value;
    Long_decimal integer, temp_integer, fractional;

    shift_Long_decimal_to_right(&temp_value_1, 1);
    shift_Long_decimal_to_right(&temp_value_2, 2);
    add_long(temp_value_1, temp_value_2, &integer);      // q - integer
    temp_integer = integer;
    // путем поочередных сдвигов получаем предцелую часть
    for(int shifts = 4; shifts <= 128; shifts *= 2)
    {
        shift_Long_decimal_to_right(&temp_integer, shifts);
        add_long(temp_integer, integer, &integer);
        if(shifts != 128)
          temp_integer = integer;
    }
    // получаем финальную целую часть
    shift_Long_decimal_to_right(&integer, 3);            // q = q >> 3;
    // получаем дробную часть
    temp_value_1 = integer;
    temp_value_2 = integer;
    shift_Long_decimal_to_left(&temp_value_1, 3);
    shift_Long_decimal_to_left(&temp_value_2, 1);
    add_long(temp_value_1, temp_value_2, &temp_integer); // ((q << 3) + (q << 1))
    sub_long(*value, temp_integer, &fractional);         // n - ((q << 3) + (q << 1))
    // получаем финальную дробную часть
    if(compare_Long_decimal(fractional, ten) != LESS)
    {
        sub_long(fractional, ten, &fractional);
        increment_mantissa_Long_decimal(&integer);
    }
    *value = integer; // "возвращаем" целую часть
    return fractional;
}

static void bank_rounding(Long_decimal * value, const Long_decimal * fractional)
{
    Long_decimal five = {{ 5, 0, 0, 0, 0, 0, 0, 0 }}; // +5 в битовом представлении
    set_exponent_Long_decimal(&five, 1);              // получаем "+0.5"

    int res_cmp = compare_Long_decimal(*fractional, five);
    if(res_cmp == GREATER) // если остаток больше 0.5, то округляем вверх (12.6 -> 13)
    {
        increment_mantissa_Long_decimal(value);
    }
    else if(res_cmp == EQUAL) // если остаток равен 0.5, то используем банковское округление (12.5 -> 12, но 13.5 -> 14)
    {
        if(get_bit_Long_decimal(value, 0) == 1) // если число нечетное
            increment_mantissa_Long_decimal(value);
    }
    // else - если остаток меньше 0.5, то оставляем число как есть (12.4 -> 12)
}

static void increment_mantissa_Long_decimal(Long_decimal * value)
{
    Long_decimal one = {{ 1, 0, 0, 0, 0, 0, 0, 0 }};
    set_exponent_Long_decimal(&one, get_exponent_Long_decimal(value));
    set_sign_Long_decimal(&one, get_sign_Long_decimal(value));
    add_long(*value, one, value);
}

static bool is_zero_Long_decimal(const Long_decimal * value)
{   // Функция считает нулем то число, мантисса которого состоит только из нулевых битов, т.е. независимо от знака и экспоненты
    bool is_true = true;
    for(int part = 0; part < PARTS_OF_LONG_DECIMAL - 1; part++)
    {
        if(value->bits[part] != 0)
        {
            is_true = false;
            break;
        }
    }
    return is_true;
}

static bool is_zero_decimal(const Decimal * value)
{
    bool is_true = true;
    for(int part = 0; part < PARTS_OF_DECIMAL - 1; part++)
    {
        if(value->bits[part] != 0)
        {
            is_true = false;
            break;
        }
    }
    return is_true;
}

static Long_decimal discard_zeroes(Long_decimal value)
{ // Функция удаляет незначащие завершающие нули
    int exponent = get_exponent_Long_decimal(&value);
    if(exponent != MIN_EXPONENT)
    {
        Long_decimal fractional;
        Long_decimal temp_value = value;
        fractional = convert_to_down_exponent(&temp_value);
        while(is_zero_Long_decimal(&fractional) && exponent != MIN_EXPONENT)
        {
            value = temp_value;
            fractional = convert_to_down_exponent(&temp_value);
            --exponent;
        }
    }
    return value;
}

static bool valid_value(float value)
{ // Функция возвращает false, если value является +-NAN или +-INF; true - если нормальное число
    unsigned long long temp = (*((unsigned long long*)(&value)) >> 23);   
    return (temp & 0xFF) != 255;
}

static bool is_float_zero(float value)
{
    bool result = false;                                 
    if((*((unsigned*)(&value)) & 0x7FFFFFFF) == 0)
        result = true;
    return result;
}

static bool is_negative_zero(float value)
{ // Функция возвращает true, если х == -0.0 (но не просто 0)
    bool result = false;                                          
    if((*((unsigned*)(&value)) & 0x7FFFFFFF) == 0 && (*((unsigned*)(&value)) >> 31) == 1)
        result = true;
    return result;
}

// Функции для работы со знаком числа

static int get_sign_decimal(const Decimal * value)
{
    return (value->bits[PARTS_OF_DECIMAL - 1] >> 31) & 0x1;
}

static int get_sign_Long_decimal(const Long_decimal * value)
{
    return (value->bits[PARTS_OF_LONG_DECIMAL - 1] >> 31) & 0x1;
}

static void set_sign_decimal(Decimal * value, int sign)
{
    if(sign) // делаем число отрицательным
        value->bits[PARTS_OF_DECIMAL - 1] |= 0x80000000; 
    else // делаем число положительным
        value->bits[PARTS_OF_DECIMAL - 1] &= 0x7FFFFFFF;
}

static void set_sign_Long_decimal(Long_decimal * value, int sign)
{
    if(sign) // делаем число отрицательным
        value->bits[PARTS_OF_LONG_DECIMAL - 1] |= 0x80000000;
    else // делаем число положительным
        value->bits[PARTS_OF_LONG_DECIMAL - 1] &= 0x7FFFFFFF;
}

// Функции для работы с экспонентой числа

static int get_exponent_decimal(const Decimal * value)
{
    return (value->bits[PARTS_OF_DECIMAL - 1] >> 16) & 0xFF;
}

static int get_exponent_Long_decimal(const Long_decimal * value)
{
    return (value->bits[PARTS_OF_LONG_DECIMAL - 1] >> 16) & 0xFF;
}

static void set_exponent_decimal(Decimal * value, int exponent)
{
    value->bits[PARTS_OF_DECIMAL - 1] &= 0xFF00FFFF;       // очищаем предыдущее значение экспоненты
    value->bits[PARTS_OF_DECIMAL - 1] |= (exponent << 16); // устанавливаем новое значение экспоненты
}

static void set_exponent_Long_decimal(Long_decimal * value, int exponent)
{
    value->bits[PARTS_OF_LONG_DECIMAL - 1] &= 0xFF00FFFF;       // очищаем предыдущее значение экспоненты
    value->bits[PARTS_OF_LONG_DECIMAL - 1] |= (exponent << 16); // устанавливаем новое значение экспоненты
}

// Функции для работы с мантиссой числа

static int get_bit_Long_decimal(const Long_decimal * value, int bit_number)
{
    int index = identify_part_of_mantissa(bit_number);
    bit_number %= 32; // получаем порядковый номер бита в value->bits[index] (т.е в диапазоне от 0 до 31)
    return (value->bits[index] >> bit_number) & 0x1;
}

static void set_bit_Long_decimal(Long_decimal * value, int bit_number, int bit)
{
    int index = identify_part_of_mantissa(bit_number);
    bit_number %= 32; // получаем порядковый номер бита в value->bits[index] (т.е в диапазоне от 0 до 31)
    if(bit) // bit == 1
        value->bits[index] |=  (0x1 << bit_number); // устанавливаем бит в значение 1
    else
        value->bits[index] &= ~(0x1 << bit_number); // устанавливаем бит в значение 0
}

static int identify_part_of_mantissa(int bit_number)
{
    int part_mantissa = 0; // if(0 <= bit_number && bit_number <= 31) // bit_number in bits[0]
    if(32 <= bit_number && bit_number <= 63)                          // bit_number in bits[1]
        part_mantissa = 1;
    else if(63 <= bit_number && bit_number <= 95)                     // bit_number in bits[2]
        part_mantissa = 2;
    // далее опеределяются индексы для частей Long_decimal
    else if(96 <= bit_number && bit_number <= 127)                    // bit_number in bits[3]
        part_mantissa = 3;
    else if(128 <= bit_number && bit_number <= 159)                   // bit_number in bits[4]
        part_mantissa = 4;
    else if(160 <= bit_number && bit_number <= 191)                   // bit_number in bits[5]
        part_mantissa = 5;
    else if(192 <= bit_number && bit_number <= 223)                   // bit_number in bits[6]
        part_mantissa = 6;
    return part_mantissa;
}