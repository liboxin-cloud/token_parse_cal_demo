#include "parser.h"

char* input = NULL;
int pos = 0;
int max_len = 16;

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

    char m_str[NUM_MAX];

    strcpy(m_str, str);
    //printf("after strcpy, m_str is %s\n", m_str);


    int decimal_len = strlen(str) - (dot_pos - str) - 1;
    if (NULL == dot_pos) {
        decimal_len = 0;
    }

    
    printf("the decimal len is %d\n", decimal_len);
    int num_len = strlen(str) + max_len - decimal_len;

    printf("the num's len is %d\n", num_len);

    for (int i = 0;i < max_len - decimal_len;i++) {
        m_str[strlen(str) + i] = 0 + '0';
    }

    printf("m_str is %s, m_str len is %d\n", m_str, (int)strlen(m_str));

    //printf("the start of the m_str is %d, the char is %c\n", strlen(str) - pos, str[strlen(str) - pos]);

    if (NULL != dot_pos) {
        big_num->decimal_pos = (dot_pos - str);
    }


    for (int i = num_len - 1;i >= 0;i--) {
        if (m_str[i] == '.') {
            continue;
        }
        big_num->digits[big_num->len++] = m_str[i] - '0';
    }

    // if (max_len >= big_num->len) {
    //     int len_temp = big_num->len;
    //     for (int i = 0;i < max_len - len_temp;i++) {
    //         big_num->digits[big_num->len++] = 0;
    //     }
    // } else {
    //     perror("the length of the num is too long\n");
    //     return;
    // }


    

    // 在 num_to_string 函数末尾添加调试信息
    printf("解析字符串: %s → digits: [", m_str);
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
            
            printf("left operator num is\n");
            print_big_num(&left);

            BigNum temp;

            /*something wrong in sub_big_num function*/
            sub_big_num(&left, &right, &temp);  // 输入是left，输出是temp
            copy_big_num(&left, &temp);
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
            printf("乘法后的right操作数是:\n");
            print_big_num(&right);

            //BigNum temp;
            multi_big_num(&left, &right, &left);
            //copy_big_num(&left, &temp);
        } else if (TOKEN_DIV == op.type) {
            BigNum right = parse_factor();
            printf("除法后的right操作数是:\n");

            //BigNum temp;
            div_big_num(&left, &right, &left, 6);
            //copy_big_num(&left, &temp);
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
    if (NULL == num1 || NULL == num2 || NULL == res) {
        perror("num1, num2 or res is NULL!!\n");
        return;
    }

    res->len = 0;
    res->sign = 1;
    res->decimal_len = 0;

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
        b_neg.sign = -b_neg.sign;
        add_big_num(num1, &b_neg, res);
        return;
    }

    int cmp = compare_abs(&a_padded, &b_padded);
    BigNum* larger = (cmp >= 0) ? &a_padded : &b_padded;
    BigNum* smaller = (larger == &a_padded) ? &b_padded : &a_padded;
    res->sign = (larger == &a_padded) ? num1->sign : -num1->sign;

    int borrow = 0;
    int max_len = (larger->len > smaller->len) ? larger->len : smaller->len;
    for (int i = 0; i < max_len; i++) {
        int digit = borrow ? (larger->digits[i] - 1) : larger->digits[i];
        if (i < smaller->len) {
            digit -= smaller->digits[i];
        }
        if (digit < 0) {
            digit += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res->digits[res->len++] = digit;
    }
    // 处理最后的借位（若有）
    if (borrow) {
        res->digits[res->len++] = 10 - borrow;
    }

    res->decimal_len = max_decimal;

    while (res->len > 1 && res->digits[res->len - 1] == 0) {
        res->len--;
    }
}


void multi_big_num(const BigNum* num1, const BigNum* num2, BigNum* res) {
    printf("------------------------------------------------\n");
    printf("multi_big_num function called\n");

    init_big_num(res);
    res->sign = num1->sign * num2->sign;
    res->decimal_len = 0;  // 补零后均为整数，乘积也为整数

    // 1. 计算补零后的乘积（原始逻辑）
    for (int i = 0; i < num1->len; i++) {
        int carry = 0;
        for (int j = 0; j < num2->len || carry > 0; j++) {
            long long product = res->digits[i + j] + carry;
            if (j < num2->len) {
                product += (long long)num1->digits[i] * num2->digits[j];
            }
            res->digits[i + j] = product % 10;
            carry = product / 10;
        }
    }

    // 2. 计算乘积的原始长度（含尾部零）
    res->len = num1->len + num2->len;
    while (res->len > 1 && res->digits[res->len - 1] == 0) {
        res->len--;
    }

    // 3. 关键修正：右移 2*max_len 位（抵消补零影响）
    int shift = 2 * max_len;  // 每个数补了max_len个零，共需右移2*max_len
    if (shift > 0) {
        // 有效数字起始位置：从shift开始（跳过前shift个零）
        int start = shift;
        // 若起始位置超过数组长度，结果为0
        if (start >= res->len) {
            res->len = 1;
            res->digits[0] = 0;
        } else {
            // 复制有效数字（从start到末尾）
            int new_len = res->len - start;
            memmove(res->digits, res->digits + start, new_len * sizeof(int));
            res->len = new_len;
            // 去除新的尾部零（若有）
            while (res->len > 1 && res->digits[res->len - 1] == 0) {
                res->len--;
            }
        }
    }

    // 4. 修正整数位长度（decimal_pos = 总长度，因无小数）
    res->decimal_pos = res->len;

    printf("now the res is\n");
    print_big_num(res);
    printf("------------------------------------------------\n");
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

    int decimal_pos = num->decimal_pos;
    //printf("当前数的小数位数是: %d\n", num->decimal_pos);
    if (0 == num->decimal_pos) {
        int index = 0;

        while (index < max_len) {
            if (0 == num->digits[index]) {
                index++;
            }
        }


        decimal_pos = num->len - index;
    }

    if (num->sign == -1 && !is_zero(num)) {
        printf("-");    

    }



    

    for (int i = num->len - 1;i >= 0;i--) {
        if (i == num->len - decimal_pos - 1) {
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