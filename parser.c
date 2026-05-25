#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "semantic.h"
#include "ast.h"

Token currentToken;
int currentScope = 0;
int nestingLevel = 0;
ASTNode *astRoot = NULL;

/* avanzar */
void advance() {
    currentToken = getNextToken();
}

/* error */
void syntaxError(const char *message) {
    printf("Error sintactico en linea %d: %s\n",
           currentToken.line,
           message);
    exit(1);
}

/* token esperado */
void expect(TokenType expected) {
    if (currentToken.type == expected) {
        advance();
    } else {
        syntaxError("Token inesperado");
    }
}

/* tipos */
int isType(TokenType type) {
    return type == TOKEN_INT ||
           type == TOKEN_FLOAT ||
           type == TOKEN_CHAR ||
           type == TOKEN_BOOL ||
           type == TOKEN_STRING;
}

/* operadores relacionales */
int isRelational(TokenType type) {
    return type == TOKEN_EQUAL ||
           type == TOKEN_NOT_EQUAL ||
           type == TOKEN_LESS ||
           type == TOKEN_GREATER ||
           type == TOKEN_LESS_EQUAL ||
           type == TOKEN_GREATER_EQUAL;
}

/* expresion */
int isArithmeticOperator(TokenType type) {
    return type == TOKEN_PLUS ||
           type == TOKEN_MINUS ||
           type == TOKEN_MULTIPLY ||
           type == TOKEN_DIVIDE ||
           type == TOKEN_MOD;
}

void parseExpression() {
    if (currentToken.type == TOKEN_IDENTIFIER ||
        currentToken.type == TOKEN_NUMBER ||
        currentToken.type == TOKEN_FLOAT_NUMBER ||
        currentToken.type == TOKEN_STRING_LITERAL ||
        currentToken.type == TOKEN_CHAR_LITERAL ||
        currentToken.type == TOKEN_TRUE ||
        currentToken.type == TOKEN_FALSE) {

        advance();
    } else {
        syntaxError("Expresion invalida");
    }

    while (isArithmeticOperator(currentToken.type)) {
        advance();

        if (currentToken.type == TOKEN_IDENTIFIER ||
            currentToken.type == TOKEN_NUMBER ||
            currentToken.type == TOKEN_FLOAT_NUMBER ||
            currentToken.type == TOKEN_STRING_LITERAL ||
            currentToken.type == TOKEN_CHAR_LITERAL ||
            currentToken.type == TOKEN_TRUE ||
            currentToken.type == TOKEN_FALSE) {

            advance();
        } else {
            syntaxError("Operando esperado");
        }
    }
}
/* condición */
void parseCondition() {
    if (currentToken.type == TOKEN_NOT) {
        advance();
    }

    parseExpression();

    if (!isRelational(currentToken.type)) {
        syntaxError("Operador relacional esperado");
    }

    advance();

    parseExpression();

    while (currentToken.type == TOKEN_AND ||
           currentToken.type == TOKEN_OR) {

        advance();

        parseExpression();

        if (!isRelational(currentToken.type)) {
            syntaxError("Operador relacional esperado");
        }

        advance();

        parseExpression();
    }
}

/* declaración */
void parseDeclaration() {
    char declaredType[20];
    char variableName[50];
    TokenType expressionType;

    if (!isType(currentToken.type)) {
        syntaxError("Tipo de dato esperado");
    }

    strcpy(declaredType, tokenTypeToDataType(currentToken.type));

    advance();

    if (currentToken.type != TOKEN_IDENTIFIER) {
        syntaxError("Identificador esperado");
    }

    strcpy(variableName, currentToken.lexeme);

    if (symbolExistsInCurrentScope(variableName, currentScope)) {
        semanticError("Variable redeclarada", currentToken.line);
    }

    advance();

    expect(TOKEN_ASSIGN);

    expressionType = currentToken.type;

    if (!areTypesCompatible(declaredType, expressionType)) {
        semanticError("Tipos incompatibles en declaracion", currentToken.line);
    }

    parseExpression();

    addSymbol(variableName, declaredType, currentScope);

    /* AST */
appendNode(&astRoot, createNode("DECLARATION", variableName));
    expect(TOKEN_SEMICOLON);
}

/* asignación */
void parseAssignment() {
    char variableName[50];

    if (currentToken.type != TOKEN_IDENTIFIER) {
        syntaxError("Identificador esperado");
    }

    strcpy(variableName, currentToken.lexeme);

    if (!symbolExists(variableName, currentScope)) {
        semanticError("Variable no declarada", currentToken.line);
    }

    advance();

    expect(TOKEN_ASSIGN);

    parseExpression();

    /* AST */
    appendNode(&astRoot, createNode("ASSIGNMENT", variableName));
    expect(TOKEN_SEMICOLON);
}

/* print */
void parsePrint() {
    char printValue[100];

    expect(TOKEN_PRINT);
    expect(TOKEN_LPAREN);

    if (currentToken.type == TOKEN_STRING_LITERAL) {
        strcpy(printValue, currentToken.lexeme);

        printf("%s\n", printValue);
    }

    parseExpression();

    appendNode(&astRoot, createNode("PRINT", printValue));

    expect(TOKEN_RPAREN);
    expect(TOKEN_SEMICOLON);
}

/* bloque */
void parseBlock();
void parseWhile();
void parseDoWhile();
void parseFor();

/* DoWhile */
void parseDoWhile() {
    expect(TOKEN_DO);

    parseBlock();

    expect(TOKEN_WHILE);
    expect(TOKEN_LPAREN);

    parseCondition();

    expect(TOKEN_RPAREN);
    expect(TOKEN_SEMICOLON);
}

/* For */
void parseFor() {
    expect(TOKEN_FOR);
    expect(TOKEN_LPAREN);

    /* declaración */
    if (!isType(currentToken.type)) {
        syntaxError("Se esperaba tipo de dato en for");
    }

    advance();

    expect(TOKEN_IDENTIFIER);
    expect(TOKEN_ASSIGN);

    parseExpression();

    expect(TOKEN_SEMICOLON);

    /* condición */
    parseCondition();

    expect(TOKEN_SEMICOLON);

    /* incremento */
    expect(TOKEN_IDENTIFIER);
    expect(TOKEN_ASSIGN);

    parseExpression();

    expect(TOKEN_RPAREN);

    parseBlock();
}

/* while */
void parseWhile() {
    expect(TOKEN_WHILE);
    expect(TOKEN_LPAREN);

    parseCondition();

    expect(TOKEN_RPAREN);

    parseBlock();
}

/* if */
void parseIf() {
    expect(TOKEN_IF);
    expect(TOKEN_LPAREN);

    parseCondition();

    expect(TOKEN_RPAREN);

    parseBlock();

    while (currentToken.type == TOKEN_ELSEIF) {
        expect(TOKEN_ELSEIF);
        expect(TOKEN_LPAREN);

        parseCondition();

        expect(TOKEN_RPAREN);

        parseBlock();
    }

    if (currentToken.type == TOKEN_ELSE) {
        expect(TOKEN_ELSE);
        parseBlock();
    }
}

/* statement */
void parseStatement() {
    if (isType(currentToken.type)) {
        parseDeclaration();
    }
    else if (currentToken.type == TOKEN_IDENTIFIER) {
        parseAssignment();
    }
    else if (currentToken.type == TOKEN_PRINT) {
        parsePrint();
    }
    else if (currentToken.type == TOKEN_IF) {
        parseIf();
    }
    else if (currentToken.type == TOKEN_WHILE) {
        parseWhile();
    }
    else if (currentToken.type == TOKEN_DO) {
        parseDoWhile();
    }
    else if (currentToken.type == TOKEN_FOR) {
        parseFor();
    }
    else {
        syntaxError("Sentencia invalida");
    }
}

/* bloque */
void parseBlock() {
    expect(TOKEN_LBRACE);

    currentScope++;
    nestingLevel++;

    if (nestingLevel > 3) {
        semanticError("Anidamiento maximo excedido", currentToken.line);
    }

    while (currentToken.type != TOKEN_RBRACE &&
           currentToken.type != TOKEN_EOF) {
        parseStatement();
    }

    expect(TOKEN_RBRACE);

    currentScope--;
    nestingLevel--;
}
/* programa */
void parseProgram() {
    expect(TOKEN_PROGRAM);

    while (currentToken.type != TOKEN_END &&
           currentToken.type != TOKEN_EOF) {
        parseStatement();
    }

    expect(TOKEN_END);

    printf("Analisis sintactico completado correctamente.\n");

    printf("\n=== AST ===\n");
    printAST(astRoot, 0);}

/* init */
void initParser(const char *filename) {
    initLexer(filename);
    initSymbolTable();
    advance();
}