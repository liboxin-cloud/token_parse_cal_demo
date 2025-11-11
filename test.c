#include "parser.h"
int main() {
    char expr[] = "7222.2343 + 8.2131";
    input = expr;
    pos = 0;
    BigNum result = parse_expr();
    print_big_num(&result);
    return 0;
}