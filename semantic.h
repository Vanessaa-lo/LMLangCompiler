#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "lexer.h"

void semanticError(const char *message, int line);
const char* tokenTypeToDataType(TokenType type);
int areTypesCompatible(const char *declaredType, TokenType expressionType);

#endif