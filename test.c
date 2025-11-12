#include "parser.h"
int main() {
    char expr[] = "72.2343 + 2.1";
    input = expr;
    pos = 0;
    BigNum result = parse_expr();
    print_big_num(&result);
    return 0;
}