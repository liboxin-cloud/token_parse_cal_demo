#include "parser.h"

char* input = NULL;
int pos = 0;
void init_big_num(BigNum* big_num) {
    for (int i = 0;i < NUM_MAX;i++) {
        big_num->digits[i] = 0;
    }

    big_num->len = 0;
    big_num->sign = 1;
    /***the decimal length of the big num, and default is integer*/
    big_num->decimal_len = 0;
}

void num_to_string(BigNum* big_num, const char* str) {
    init_big_num(big_num);
    int start_pos = 0;
    /*the point of the decimal dot*/
    const char* dot_pos = strchr(str, '.');

    /*tell the sign of the num*/
    if ('-' == str[0]) {
        big_num->sign = -1;
        start_pos = 1;
    }


    /*handle the decimal and integer part of the num*/
    int integer_end_pos = (dot_pos != NULL) ? (dot_pos - str) : strlen(str);
    /*reverse the big_num string and store every num into the array*/
    for (int i = strlen(str) - 1;i >= start_pos;i--) {
        if (str[i] < '0' || str[i] > '9') {
            break;
        }

        big_num->digits[big_num->len++] = str[i] - '0';
    }

    if (NULL != dot_pos) {
        int decimal_start_pos = integer_end_pos + 1;

        for (int i = strlen(str) - 1;i >= decimal_start_pos;i--) {
            if (str[i] < '0' || str[i] > '9') {
                break;
            }

            big_num->digits[big_num->len++] = str[i] - '0';
            big_num->decimal_len++;
        }

        /*reverse the decimal part of the num*/
        for (int i = 0; i < big_num->decimal_len / 2; i++) {
            int swap_idx = big_num->len - big_num->decimal_len + i;
            int temp = big_num->digits[swap_idx];
            big_num->digits[swap_idx] = big_num->digits[big_num->len - 1 - i];
            big_num->digits[big_num->len - 1 - i] = temp;
        }
    }

    while (big_num->decimal_len > 0 && 
        big_num->digits[big_num->decimal_len - 1] == 0) {
        
        big_num->decimal_len--;
        big_num->len--;
    }


}


bool is_zero(const BigNum* num) {
    for (int i = 0; i < num->len; i++) {
        if (num->digits[i] != 0) return false;
    }
    return true;
}

Token next_token() {
    Token token;

    /*skip the blank space*/
    while (' ' == input[pos]) {
        pos++;
    }


    /*parse the num*/
    if (input[pos] >= '0' && input[pos] <= '9' ||
        '.' == input[pos]) {
        char numStr[NUM_MAX];

        /*define the counter i*/
        int i = 0;
        while (input[pos] >= '0' && input[pos] <= '9' ||
        '.' == input[pos]) {
            numStr[i++] = input[pos++];
        }

        numStr[i] = '\0';
        num_to_string(&token.num, numStr);
        token.type = TOKEN_NUM;

    } else {
        /*parse the operator*/
        switch (input[pos++]) {
            case '+': { token.type = TOKEN_PLUS; break; }
            case '-': { token.type = TOKEN_MINUS; break; }
            case '*': { token.type = TOKEN_MUL; break; }
            case '/': { token.type = TOKEN_DIV; break; }
            case '(': { token.type = TOKEN_LPAREN; break; }
            case ')': { token.type = TOKEN_RPAREN; break; }
            case '\0': { token.type = TOKEN_EOF; break;}
            default: { perror("unknown operator\n"); /* error parse */ break; }
        }
    }

    return token;
}


void add_big_num(const BigNum* num1, const BigNum* num2, BigNum* res) {
    if (NULL == num1 || NULL == num2) {
        perror("num1 or num2 is NULL!!\n");
        return;
    }

    init_big_num(res);
    res->sign = num1->sign;

    /*get the max decimal length of the two num*/
    int max_decimal = (num1->decimal_len > num2->decimal_len) ?
    num1->decimal_len : num2->decimal_len;

    BigNum a_padded = *num1;
    BigNum b_padded = *num2;

    /***pad the decimal part of the num with 0*/
    while (a_padded.decimal_len < max_decimal) {
        a_padded.digits[a_padded.len++] = 0;
        a_padded.decimal_len++;
    }

    while (b_padded.decimal_len < max_decimal) {
        b_padded.digits[b_padded.len++] = 0;
        b_padded.decimal_len++;
    }

    /*if the sign is same, add them, if not, subtract them*/

    if (num1->sign == num2->sign) {
        int carry = 0;
        int max_len = (a_padded.len > b_padded.len) ? a_padded.len : b_padded.len;
    
        for (int i = 0;i < max_len;i++) {
            int sum = carry;

            if (i < a_padded.len) { 
                sum += a_padded.digits[i];
            }
            if (i < b_padded.len) {
                sum += b_padded.digits[i];
            }

            res->digits[res->len++] = sum % 10;
            carry = sum / 10;
        }
        if (carry > 0) {
            res->digits[res->len++] = carry;
        }
        res->decimal_len = max_decimal;
    } else {
        BigNum b_neg = *num2;
        b_neg.sign *= -1;
        sub_big_num(num1, &b_neg, res);
        return;
    }
}

BigNum parse_factor() {
    Token token = next_token();
    if (TOKEN_NUM == token.type) {
        return token.num;
    } else if (TOKEN_LPAREN == token.type) {
        BigNum expr = parse_expr();

        if (TOKEN_RPAREN != next_token().type) {
            perror("missing right parenthesis\n");
        }

        return expr;
    } else {
        perror("missing factor\n");
    }

    BigNum res;
    return res;
}


BigNum parse_expr() {
    BigNum left = parse_term();

    while (true) {
        Token op = next_token();
        if (TOKEN_PLUS == op.type) {
            BigNum right = parse_term();
            add_big_num(&left, &right, &left);
        } else if (TOKEN_MINUS == op.type) {
            BigNum right = parse_term();
            sub_big_num(&left, &right, &left);
        } else {
            pos--;
            break;
        }
    }

    return left;
}

BigNum parse_term() {
    /*parse one factor*/
    BigNum left = parse_factor();

    while (true) {
        Token op = next_token();

        if (TOKEN_MUL == op.type) {
            BigNum right = parse_factor();
            multi_big_num(&left, &right, &left);
        } else if (TOKEN_DIV == op.type) {
            BigNum right = parse_factor();
            div_big_num(&left, &right, &left, 6);
        } else {
            pos--;
            break;
        }
    }

    return left;
}

int compare_abs(const BigNum* num1, const BigNum* num2) {
    if (num1->len > num2->len) {
        return 1;
    }
    if (num1->len < num2->len) {
        return -1;
    }

    for (int i = num1->len - 1;i >= 0;i--) {
        if (num1->digits[i] > num2->digits[i]) {
            return 1;
        } else if (num1->digits[i] < num2->digits[i]) {
            return -1;
        }
    }

    return 0;
}


void sub_big_num(const BigNum* num1, const BigNum* num2, BigNum* res) {
    if (NULL == num1 || NULL == num2) {
        perror("num1 or num2 is NULL!!\n");
        return;
    }

    int max_decimal = (num1->decimal_len > num2->decimal_len) ? num1->decimal_len : num2->decimal_len;
    BigNum a_padded = *num1;
    BigNum b_padded = *num2;
    while (a_padded.decimal_len < max_decimal) {
        a_padded.digits[a_padded.len++] = 0;
        a_padded.decimal_len++;
    }
    while (b_padded.decimal_len < max_decimal) {
        b_padded.digits[b_padded.len++] = 0;
        b_padded.decimal_len++;
    }

    if (num1->sign != num2->sign) {
        BigNum b_neg = *num2;
        b_neg.sign *= -1;
        add_big_num(num1, &b_neg, res);
        return;
    }

    /*the sign is same*/

    BigNum* larger = (compare_abs(&a_padded, &b_padded) >= 0) ? &a_padded : &b_padded;
    BigNum* smaller = (larger == &a_padded) ? &b_padded : &a_padded;
    res->sign = (larger == &a_padded) ? num1->sign : -num1->sign;

    int borrow = 0;
    for (int i = 0; i < larger->len; i++) {
        int digit = larger->digits[i] - borrow;
        if (i < smaller->len) digit -= smaller->digits[i];
        if (digit < 0) {
            digit += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res->digits[res->len++] = digit;
    }
    res->decimal_len = max_decimal;

    /*remove the front zero*/
    while (res->len > 1 && res->digits[res->len - 1] == 0) {
        res->len--;
    }
}

void multi_big_num(const BigNum* num1, const BigNum* num2, BigNum* res) {
    init_big_num(res);
    res->sign = num1->sign * num2->sign;
    res->decimal_len = num1->decimal_len + num2->decimal_len;  // 小数位数相加

    for (int i = 0; i < num1->len; i++) {
        int carry = 0;
        for (int j = 0; j < num2->len || carry; j++) {
            long long product = res->digits[i + j] + carry;
            if (j < num2->len) product += (long long)num1->digits[i] * num2->digits[j];
            res->digits[i + j] = product % 10;
            carry = product / 10;
        }
        res->len = (i + num2->len > res->len) ? (i + num2->len) : res->len;
    }

    while (res->decimal_len > 0 && res->digits[res->decimal_len - 1] == 0) {
        res->decimal_len--;
        res->len--;
    }

    while (res->len > res->decimal_len + 1 && res->digits[res->len - 1] == 0) {
        res->len--;
    }
}


// 辅助函数：给大整数补零（用于除法扩展小数位）
void pad_zero(BigNum* num, int zeros) {
    for (int i = 0; i < zeros; i++) {
        num->digits[num->len++] = 0;
    }
}

void div_big_num(const BigNum* num1, const BigNum* num2, BigNum* res, int max_decimal) {
    init_big_num(res);
    res->sign = num1->sign * num2->sign;
    res->decimal_len = max_decimal;  // 结果保留max_decimal位小数

    // 处理除数为0
    if (is_zero(num2)) {
        perror("division by zero");
        return;
    }

    // 转为整数除法：(a * 10^max_decimal) / b
    BigNum a_padded = *num1;
    BigNum b_padded = *num2;
    // 对齐被除数和除数的小数位，并补零扩展到max_decimal位
    int shift = b_padded.decimal_len - a_padded.decimal_len + max_decimal;
    if (shift > 0) pad_zero(&a_padded, shift);
    else pad_zero(&b_padded, -shift);

    // 去除小数点，按整数除法计算
    BigNum quotient, remainder;
    integer_div(&a_padded, &b_padded, &quotient, &remainder);

    // 商的长度需至少为max_decimal（不足补零）
    while (quotient.len < max_decimal) {
        quotient.digits[quotient.len++] = 0;
    }

    // 复制商到结果（整数部分为len - decimal_len，小数部分为decimal_len）
    res->len = quotient.len;
    memcpy(res->digits, quotient.digits, quotient.len * sizeof(int));

    // 四舍五入（简化处理：最后一位 >=5 则进1）
    if (res->len > res->decimal_len && res->digits[res->decimal_len] >= 5) {
        int carry = 1;
        for (int i = res->decimal_len - 1; i >= 0 && carry; i--) {
            res->digits[i] += carry;
            if (res->digits[i] >= 10) {
                res->digits[i] %= 10;
                carry = 1;
            } else {
                carry = 0;
            }
        }
        if (carry) {
            res->digits[res->len++] = 1;  // 进位到整数部分
        }
    }
}

void integer_div(const BigNum* num1, const BigNum* num2, BigNum* quotient, BigNum* remainder) {
    init_big_num(quotient);
    init_big_num(remainder);
    quotient->decimal_len = 0;  // 商为整数
    remainder->decimal_len = 0; // 余数为整数

    // 若被除数小于除数，商为0，余数为被除数
    if (compare_abs(num1, num2) < 0) {
        copy_big_num(remainder, num1);
        return;
    }

    BigNum dividend;
    copy_big_num(&dividend, num1);  // 复制被除数，避免修改原数据

    // 计算商的位数（被除数长度 - 除数长度）
    int shift = dividend.len - num2->len;
    BigNum divisor_shifted;
    copy_big_num(&divisor_shifted, num2);

    // 将除数左移 shift 位（相当于乘以 10^shift），与被除数对齐
    if (shift > 0) {
        for (int i = divisor_shifted.len - 1; i >= 0; i--) {
            divisor_shifted.digits[i + shift] = divisor_shifted.digits[i];
        }
        for (int i = 0; i < shift; i++) {
            divisor_shifted.digits[i] = 0;
        }
        divisor_shifted.len += shift;
    }

    // 逐位计算商
    for (int i = 0; i <= shift; i++) {
        int count = 0;
        // 循环减去除数，统计次数（即当前位的商）
        while (compare_abs(&dividend, &divisor_shifted) >= 0) {
            BigNum temp;
            sub_abs(&dividend, &divisor_shifted, &temp);
            copy_big_num(&dividend, &temp);
            count++;
        }
        // 记录当前位的商（从高位到低位存储）
        quotient->digits[shift - i] = count;
        quotient->len = shift - i + 1;

        // 除数右移一位（相当于除以 10），准备计算下一位商
        for (int j = 0; j < divisor_shifted.len - 1; j++) {
            divisor_shifted.digits[j] = divisor_shifted.digits[j + 1];
        }
        divisor_shifted.len--;
        // 去除右移后的前导零
        while (divisor_shifted.len > 0 && divisor_shifted.digits[divisor_shifted.len - 1] == 0) {
            divisor_shifted.len--;
        }
    }

    // 最终余数为被除数剩余值
    copy_big_num(remainder, &dividend);
}

void copy_big_num(BigNum* num1, const BigNum* num2) {
    init_big_num( num1);
    num1->sign = num2->sign;
    num1->len = num2->len;
    num1->decimal_len = num2->decimal_len;

    for (int i = 0; i < num2->len; i++) {
        num1->digits[i] = num2->digits[i];
    }

}

void print_big_num(const BigNum* num) {
    if (num->sign == -1 && !is_zero(num)) {
        printf("-");
    }

    int integer_len = num->len - num->decimal_len;
    if (integer_len <= 0) {
        printf("0");  // 整数部分为0
    } else {
        // 打印整数部分（高位到低位）
        for (int i = num->len - 1; i >= num->decimal_len; i--) {
            printf("%d", num->digits[i]);
        }
    }

    // 打印小数部分
    if (num->decimal_len > 0) {
        printf(".");
        for (int i = num->decimal_len - 1; i >= 0; i--) {
            printf("%d", num->digits[i]);
        }
    }
    printf("\n");
}

void sub_abs(const BigNum* num1, const BigNum* num2, BigNum* res) {
    init_big_num(res);
    res->decimal_len = 0;
    int borrow = 0;
    for (int i = 0; i < num1->len; i++) {
        int digit = num1->digits[i] - borrow;
        if (i < num2->len) digit -= num2->digits[i];
        if (digit < 0) { digit += 10; borrow = 1; }
        else borrow = 0;
        res->digits[res->len++] = digit;
    }
    while (res->len > 1 && res->digits[res->len - 1] == 0) res->len--;
}