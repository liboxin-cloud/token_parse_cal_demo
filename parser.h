#include <stdio.h>
#include <stdbool.h>

#define NUM_MAX 1024

char* input;
int pos;

/*the struct of the big_num*/
typedef struct BigNum {
    int digits[NUM_MAX];    /*the big num array*/
    int len;                /*the length of the big_num*/
    int sign;               /*the sign of the big num*/
}BigNum;


/*token type*/
typedef enum token_type {
    TOKEN_NUM,    // digit number
    TOKEN_PLUS,   // +
    TOKEN_MINUS,  // -
    TOKEN_MUL,    // *
    TOKEN_DIV,    // /
    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )
    TOKEN_EOF     // end signal
}token_type;

/*the struct of the token*/
typedef struct Token {
    token_type type;
    BigNum num;
}Token;

/*init the big_num*/
void init_big_num(BigNum* big_num);

/*transform the big_num string to the digit*/
void num_to_string(BigNum* big_num, const char* str);

/*extract the token from the expression string*/
Token next_token();

/*the add of the big num*/
void add_big_num(const BigNum* num1, const BigNum* num2, BigNum* res);

/*the sub of the big num*/
void sub_big_num(const BigNum* num1, const BigNum* num2, BigNum* res);

/*the multi of the big num*/
void multi_big_num(const BigNum* num1, const BigNum* num2, BigNum* res);

/*the div of the big num*/
void div_big_num(const BigNum* num1, const BigNum* num2, BigNum* res);

/*parse the factors*/
BigNum parse_factor();

/*parse the expression*/
BigNum parse_expr();

/*parse the term*/
BigNum parse_term();

