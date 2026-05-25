#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

/* error semántico */
void semanticError(const char *message, int line) {
    printf("Error semantico en linea %d: %s\n", line, message);
    exit(1);
}

/* convertir token a tipo */
const char* tokenTypeToDataType(TokenType type) {
    switch(type) {
        case TOKEN_INT:
            return "int";

        case TOKEN_FLOAT:
            return "float";

        case TOKEN_CHAR:
            return "char";

        case TOKEN_BOOL:
            return "bool";

        case TOKEN_STRING:
            return "string";

        default:
            return "unknown";
    }
}

/* validar compatibilidad */
int areTypesCompatible(const char *declaredType, TokenType expressionType) {
    if (strcmp(declaredType, "int") == 0 &&
        expressionType == TOKEN_NUMBER) {
        return 1;
    }

    if (strcmp(declaredType, "float") == 0 &&
        (expressionType == TOKEN_FLOAT_NUMBER ||
         expressionType == TOKEN_NUMBER)) {
        return 1;
    }

    if (strcmp(declaredType, "char") == 0 &&
        expressionType == TOKEN_CHAR_LITERAL) {
        return 1;
    }

    if (strcmp(declaredType, "bool") == 0 &&
        (expressionType == TOKEN_TRUE ||
         expressionType == TOKEN_FALSE)) {
        return 1;
    }

    if (strcmp(declaredType, "string") == 0 &&
        expressionType == TOKEN_STRING_LITERAL) {
        return 1;
    }

    return 0;
}