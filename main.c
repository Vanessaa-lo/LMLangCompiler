#include <stdio.h>
#include "ast.h"

extern ASTNode *astRoot;

int main() {
    initParser("test_ok.lm");
    parseProgram();

    freeAST(astRoot);

    return 0;
}