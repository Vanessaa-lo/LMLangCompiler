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

    /* Error lexico */
    if (currentToken.type == TOKEN_ERROR) {

        printf("Error lexico en linea %d: %s\n",
               currentToken.line,
               currentToken.lexeme);

        exit(1);
    }
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
        if (expected == TOKEN_END) {
            syntaxError("Se esperaba 'end' al final del programa");
        }
        else if (expected == TOKEN_SEMICOLON) {
            syntaxError("Se esperaba ';'");
        }
        else if (expected == TOKEN_RPAREN) {
            syntaxError("Se esperaba ')'");
        }
        else if (expected == TOKEN_LPAREN) {
            syntaxError("Se esperaba '('");
        }
        else if (expected == TOKEN_RBRACE) {
            syntaxError("Se esperaba '}'");
        }
        else if (expected == TOKEN_LBRACE) {
            syntaxError("Se esperaba '{'");
        }
        else if (expected == TOKEN_IDENTIFIER) {
            syntaxError("Se esperaba un identificador");
        }
        else if (expected == TOKEN_ASSIGN) {
            syntaxError("Se esperaba '='");
        }
        else {
            syntaxError("Token inesperado");
        }
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

/* prototipos */
TokenType parseExpression();
TokenType parseTerm();
TokenType parseFactor();

/* factor */
TokenType parseFactor() {

    TokenType type;

    /* identificador */
    if (currentToken.type == TOKEN_IDENTIFIER) {

type = getVariableType(currentToken.lexeme,
                       currentScope);

    if (type == TOKEN_ERROR) {

        semanticError(
            "Variable no declarada",
            currentToken.line
        );
    }

        type = TOKEN_INT;

        advance();

        return type;
    }

    /* enteros */
    else if (currentToken.type == TOKEN_NUMBER) {

        advance();

        return TOKEN_INT;
    }

    /* flotantes */
    else if (currentToken.type == TOKEN_FLOAT_NUMBER) {

        advance();

        return TOKEN_FLOAT;
    }

    /* strings */
    else if (currentToken.type == TOKEN_STRING_LITERAL) {

        advance();

        return TOKEN_STRING;
    }

    /* chars */
    else if (currentToken.type == TOKEN_CHAR_LITERAL) {

        advance();

        return TOKEN_CHAR;
    }

    /* bool */
    else if (currentToken.type == TOKEN_TRUE ||
             currentToken.type == TOKEN_FALSE) {

        advance();

        return TOKEN_BOOL;
    }

    /* parentesis */
    else if (currentToken.type == TOKEN_LPAREN) {

        advance();

        type = parseExpression();

        expect(TOKEN_RPAREN);

        return type;
    }

    else {
        syntaxError("Factor invalido");
    }

    return TOKEN_ERROR;
}

/* termino */
TokenType parseTerm() {

    TokenType leftType;
    TokenType rightType;
    TokenType operatorType;

    leftType = parseFactor();

    while (currentToken.type == TOKEN_MULTIPLY ||
           currentToken.type == TOKEN_DIVIDE ||
           currentToken.type == TOKEN_MOD) {

        operatorType = currentToken.type;

        advance();

        rightType = parseFactor();

        /* Validacion semantica */

        /* No permitir strings */
        if (leftType == TOKEN_STRING ||
            rightType == TOKEN_STRING) {

            semanticError(
                "Operacion invalida con strings",
                currentToken.line
            );
        }

        /* No permitir bool */
        if (leftType == TOKEN_BOOL ||
            rightType == TOKEN_BOOL) {

            semanticError(
                "Operacion invalida con bool",
                currentToken.line
            );
        }

        /* Division entre cero */
        if (operatorType == TOKEN_DIVIDE &&
            rightType == TOKEN_NUMBER) {

            /*
               Aun no evaluamos valores reales,
               esto se mejorara despues
            */
        }

        /* float domina */
        if (leftType == TOKEN_FLOAT ||
            rightType == TOKEN_FLOAT) {

            leftType = TOKEN_FLOAT;
        }
        else {
            leftType = TOKEN_INT;
        }
    }

    return leftType;
}
/* expresion */
TokenType parseExpression() {

    TokenType leftType;
    TokenType rightType;
    TokenType operatorType;

    leftType = parseTerm();

    while (currentToken.type == TOKEN_PLUS ||
           currentToken.type == TOKEN_MINUS) {

        operatorType = currentToken.type;

        advance();

        rightType = parseTerm();

        /* Strings */

        if (leftType == TOKEN_STRING ||
            rightType == TOKEN_STRING) {

            /* Solo permitir concatenacion con + */
            if (operatorType != TOKEN_PLUS) {

                semanticError(
                    "Operacion invalida con strings",
                    currentToken.line
                );
            }

            leftType = TOKEN_STRING;
        }

        /* Bool */

        else if (leftType == TOKEN_BOOL ||
                 rightType == TOKEN_BOOL) {

            semanticError(
                "Operacion invalida con bool",
                currentToken.line
            );
        }

        /* float domina */

        else if (leftType == TOKEN_FLOAT ||
                 rightType == TOKEN_FLOAT) {

            leftType = TOKEN_FLOAT;
        }

        /* enteros */

        else {

            leftType = TOKEN_INT;
        }
    }

    return leftType;
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

    /* obtener tipo real de la expresion */
    expressionType = parseExpression();

    /* validar compatibilidad */
    if (!areTypesCompatible(declaredType,
                            expressionType)) {

        semanticError(
            "Tipos incompatibles en declaracion",
            currentToken.line
        );
    }

    addSymbol(variableName, declaredType, currentScope);

    printf("Declaracion encontrada: %s\n",
        variableName);    

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

    if (currentToken.type == TOKEN_EOF) {
        syntaxError("Se esperaba 'end' al final del programa");
    }

    expect(TOKEN_END);

    printf("Analisis sintactico completado correctamente.\n");

    printf("\n=== AST ===\n");
    printAST(astRoot, 0);
}

/* init */
void initParser(const char *filename) {
    initLexer(filename);
    initSymbolTable();
    advance();
}