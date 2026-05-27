#include <stdio.h>
#include "parser.h"
#include "ast.h"

extern ASTNode *astRoot;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo_fuente>\n", argv[0]);
        return 1;
    }

    initParser(argv[1]);
    parseProgram();

    freeAST(astRoot);

    return 0;
}