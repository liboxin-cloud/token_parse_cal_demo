#include "parser.h"

void init_big_num(BigNum* big_num) {
    for (int i = 0;i < NUM_MAX;i++) {
        big_num->digits[i] = 0;
    }

    big_num->len = 0;
    big_num->sign = 1;
}

void num_to_string(BigNum* big_num, const char* str) {
    init_big_num(big_num);
    int start_pos = 0;

    /*tell the sign of the num*/
    if ('-' == str[0]) {
        big_num->sign = -1;
        start_pos = 1;
    }

    /*reverse the big_num string and store every num into the array*/
    for (int i = strlen(str) - 1;i >= start_pos;i--) {
        big_num->digits[big_num->len++] = str[i] - '0';
    }
}

Token next_token() {
    Token token;

    /*skip the blank space*/
    while (' ' == input[pos]) {
        pos++;
    }


    /*parse the num*/
    if (input[pos] >= '0' && input[pos] <= '9') {
        char numStr[NUM_MAX];

        /*define the counter i*/
        int i = 0;
        while (input[pos] >= '0' && input[pos] <= '9') {
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

    int carry = 0;
    int maxLen = num1->len > num2->len ? num1->len : num2->len;

    for (int i = 0;i < maxLen;i++) {
        int sum = carry;

        if (i < num1->len) {
            sum += num1->digits[i];

        }
        if (i < num2->len) {
            sum += num2->digits[i];
        }

        res->digits[res->len++] = sum % 10;
        carry = sum / 10;
    }
}

BigNum parse_factor() {
    Token token = next_token();
    if (TOKEN_NUM == token.type) {
        return token.num;
    } else if (TOKEN_LPAREN == token.type) {
        BigNum expr = parse_expr();
    }
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
        } else if (TOKEN_MUL == op.type) {
            BigNum right = parse_term();
            multi_big_num(&left, &right, &left);
        } else if (TOKEN_DIV == op.type) {
            BigNum right = parse_term();
            div_big_num(&left, &right, &left);
        } else {
            pos--;
            break;
        }
    }

    return left;

}