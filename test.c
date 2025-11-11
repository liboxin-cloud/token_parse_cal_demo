#include "parser.h"
int main() {
    char expr[] = "1.45 + 67.89";
    input = expr;
    pos = 0;
    BigNum result = parse_expr();
    print_big_num(&result);
    return 0;
}