#include "parser.h"

char* input = NULL;
int pos = 0;
int max_len = 0;

void init_big_num(BigNum* big_num) {
    for (int i = 0;i < NUM_MAX;i++) {
        big_num->digits[i] = 0;
    }

    big_num->len = 0;
    big_num->sign = 1;
    /***the decimal length of the big num, and default is integer*/
    big_num->decimal_len = 0;
    big_num->decimal_pos = 0;
}

void num_to_string(BigNum* big_num, const char* str) {
    init_big_num(big_num);
    int start_pos = 0;
    const char* dot_pos = strchr(str, '.');

    if (str[0] == '-') {
        big_num->sign = -1;
        start_pos = 1;
    }

    // for (int i = strlen(str) - 1;i >= 0;i--) {
    //     big_num->digits[big_num->decimal_len++] = str[i];
    // }

    if (NULL != dot_pos) {
        big_num->decimal_pos = (dot_pos - str);
    }


    for (int i = strlen(str) - 1;i >= 0;i--) {
        if (str[i] == '.') {
            continue;
        }
        big_num->digits[big_num->len++] = str[i] - '0';
    }


    

    // 在 num_to_string 函数末尾添加调试信息
    printf("解析字符串: %s → digits: [", str);
    for (int i = 0; i < big_num->len; i++) {
        printf("%d,", big_num->digits[i]);
    }
    printf("], len=%d, decimal_len=%d, decimal_pos: %d\n", big_num->len, big_num->decimal_len, big_num->decimal_pos);
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

    printf("add big_num function is called\n");

    init_big_num(res);
    res->sign = num1->sign;

    /*get the max decimal length of the two num*/
    int max_decimal = (num1->decimal_len > num2->decimal_len) ?
    num1->decimal_len : num2->decimal_len;

    BigNum a_padded;
    copy_big_num(&a_padded, num1);
    BigNum b_padded;
    copy_big_num(&b_padded, num2);

    printf("a_padded is:\n");
    print_big_num(&a_padded);

    printf("b_padded is:\n");
    print_big_num(&b_padded);

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
        int max_decimal_len = ((a_padded.len - a_padded.decimal_pos) > (b_padded.len - b_padded.decimal_pos)) ?
        (a_padded.len - a_padded.decimal_pos) : (b_padded.len - b_padded.decimal_pos);
    
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
        res->decimal_pos = (num1->decimal_pos > num2->decimal_pos) ? 
        num1->decimal_pos : num2->decimal_pos;

        printf("res decimal pos is %d\n", res->decimal_pos);

    } else {
        BigNum b_neg = *num2;
        b_neg.sign *= -1;
        sub_big_num(num1, &b_neg, res);
        return;
    }

    printf("add res is \n");
    print_big_num(res);
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

    printf("\n 解析term后left的值是:");

    print_big_num(&left);

    while (true) {
        Token op = next_token();
        if (TOKEN_PLUS == op.type) {
            BigNum right = parse_term();
            printf("加法后的右操作数:\n");
            print_big_num(&right);

            // 关键：用临时变量存储结果，避免覆盖left
            BigNum temp;
            add_big_num(&left, &right, &temp);  // 输入是left，输出是temp
            copy_big_num(&left, &temp);  // 安全拷贝结果回left
        } else if (TOKEN_MINUS == op.type) {
            BigNum right = parse_term();
            printf("减法后的右操作数:\n");
            print_big_num(&right);
            sub_big_num(&left, &right, &left);
        } else {
            pos--;
            break;
        }
    }

    printf("\n 解析expr后left的值是:");

    print_big_num(&left);

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
    res->decimal_len = num1->decimal_len + num2->decimal_len; 

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



void pad_zero(BigNum* num, int zeros) {
    for (int i = 0; i < zeros; i++) {
        num->digits[num->len++] = 0;
    }
}

void div_big_num(const BigNum* num1, const BigNum* num2, BigNum* res, int max_decimal) {
    init_big_num(res);
    res->sign = num1->sign * num2->sign;
    res->decimal_len = max_decimal;


    if (is_zero(num2)) {
        perror("division by zero");
        return;
    }


    BigNum a_padded = *num1;
    BigNum b_padded = *num2;

    int shift = b_padded.decimal_len - a_padded.decimal_len + max_decimal;
    if (shift > 0) pad_zero(&a_padded, shift);
    else pad_zero(&b_padded, -shift);


    BigNum quotient, remainder;
    integer_div(&a_padded, &b_padded, &quotient, &remainder);


    while (quotient.len < max_decimal) {
        quotient.digits[quotient.len++] = 0;
    }


    res->len = quotient.len;
    memcpy(res->digits, quotient.digits, quotient.len * sizeof(int));


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
            res->digits[res->len++] = 1;
        }
    }
}

void integer_div(const BigNum* num1, const BigNum* num2, BigNum* quotient, BigNum* remainder) {
    init_big_num(quotient);
    init_big_num(remainder);
    quotient->decimal_len = 0;
    remainder->decimal_len = 0;


    if (compare_abs(num1, num2) < 0) {
        copy_big_num(remainder, num1);
        return;
    }

    BigNum dividend;
    copy_big_num(&dividend, num1);


    int shift = dividend.len - num2->len;
    BigNum divisor_shifted;
    copy_big_num(&divisor_shifted, num2);


    if (shift > 0) {
        for (int i = divisor_shifted.len - 1; i >= 0; i--) {
            divisor_shifted.digits[i + shift] = divisor_shifted.digits[i];
        }
        for (int i = 0; i < shift; i++) {
            divisor_shifted.digits[i] = 0;
        }
        divisor_shifted.len += shift;
    }


    for (int i = 0; i <= shift; i++) {
        int count = 0;

        while (compare_abs(&dividend, &divisor_shifted) >= 0) {
            BigNum temp;
            sub_abs(&dividend, &divisor_shifted, &temp);
            copy_big_num(&dividend, &temp);
            count++;
        }

        quotient->digits[shift - i] = count;
        quotient->len = shift - i + 1;


        for (int j = 0; j < divisor_shifted.len - 1; j++) {
            divisor_shifted.digits[j] = divisor_shifted.digits[j + 1];
        }
        divisor_shifted.len--;

        while (divisor_shifted.len > 0 && divisor_shifted.digits[divisor_shifted.len - 1] == 0) {
            divisor_shifted.len--;
        }
    }

    copy_big_num(remainder, &dividend);
}

void copy_big_num(BigNum* dest, const BigNum* src) {
    init_big_num(dest);

    dest->sign = src->sign;
    dest->len = src->len;
    dest->decimal_len = src->decimal_len;
    dest->decimal_pos = src->decimal_pos;

    /*copy_big_num(&a_padded, num1);*/

    for (int i = 0; i < src->len; i++) {
        dest->digits[i] = src->digits[i];
    }

    // printf("copy_big_num src: digits=[");
    // for (int i=0; i<src->len; i++) printf("%d,", src->digits[i]);
    // printf("], len=%d, decimal_len=%d\n", src->len, src->decimal_len);

    // printf("拷贝后dest: digits=[");
    // for (int i = 0; i < dest->len; i++) {
    //     printf("%d,", dest->digits[i]);
    // }
    // printf("], len=%d, decimal_len=%d\n", dest->len, src->decimal_len);
}

void print_big_num(const BigNum* num) {
    if (num->sign == -1 && !is_zero(num)) {
        printf("-");    
    }

    // int integer_len = num->len - num->decimal_len;


    // // 打印整数部分（存储时低位在前，打印时需从高位到低位）
    // if (integer_len <= 0) {
    //     printf("0");  // 整数部分为0
    // } else {
    //     // 整数部分存储在 digits[decimal_len ... len-1]，需逆序打印
    //     for (int i = integer_len - 1; i >= 0; i--) {
    //         printf("%d", num->digits[i]);
    //     }
    // }

    // // 打印小数部分（存储时高位在前，直接顺序打印）
    // if (num->decimal_pos > 0) {
    //     printf(".");
    //     // 小数部分存储在 digits[0 ... decimal_len-1]，直接按顺序打印
    //     for (int i = integer_len; i < num->len; i++) {
    //         printf("%d", num->digits[i]);
    //     }
    // }



    for (int i = num->len - 1;i >= 0;i--) {
        if (i == num->len - num->decimal_pos - 1) {
            printf(".");
        }
        printf("%d", num->digits[i]);
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