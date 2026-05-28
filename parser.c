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
/* nodo temporal de expresion */
ASTNode *expressionNode = NULL;

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

    /* negacion logica */
if (currentToken.type == TOKEN_NOT) {

    advance();

    type = parseFactor();

    /* solo bool */
    if (type != TOKEN_BOOL) {

        semanticError(
            "Negacion logica invalida",
            currentToken.line
        );
    }

    return TOKEN_BOOL;
}

/* negativo */
if (currentToken.type == TOKEN_MINUS) {

        advance();

        type = parseFactor();

        /* no permitir string */
        if (type == TOKEN_STRING) {

            semanticError(
                "Negativo invalido para string",
                currentToken.line
            );
        }

        /* no permitir bool */
        if (type == TOKEN_BOOL) {

            semanticError(
                "Negativo invalido para bool",
                currentToken.line
            );
        }

        return type;
    }

     /* identificador */
    if (currentToken.type == TOKEN_IDENTIFIER) {

    char variableName[50];

    strcpy(variableName, currentToken.lexeme);

    /* obtener tipo real */
    type = getVariableType(variableName,
                        currentScope);

    if (type == TOKEN_ERROR) {

        semanticError(
            "Variable no declarada",
            currentToken.line
        );
    }

    /* verificar inicializacion */
    if (!isInitialized(variableName,
                    currentScope)) {

        semanticError(
            "Variable usada sin inicializar",
            currentToken.line
        );
    }
    expressionNode =
        createValueNode(
            "IDENTIFIER",
            variableName
        );

    advance();

    return type;
    }
    /* enteros */
    else if (currentToken.type == TOKEN_NUMBER) {

        expressionNode =
            createValueNode(
                "NUMBER",
                currentToken.lexeme
            );

        advance();

        return TOKEN_INT;
    }
    /* flotantes */
    else if (currentToken.type == TOKEN_FLOAT_NUMBER) {

    expressionNode =
        createValueNode(
            "FLOAT",
            currentToken.lexeme
        );
        advance();

        return TOKEN_FLOAT;
    }

    /* strings */
    else if (currentToken.type == TOKEN_STRING_LITERAL) {

        expressionNode =
            createValueNode(
                "STRING",
                currentToken.lexeme
            );

        advance();

        return TOKEN_STRING;
    }

    /* chars */
    else if (currentToken.type == TOKEN_CHAR_LITERAL) {

        expressionNode =
            createValueNode(
                "CHAR",
                currentToken.lexeme
            );
        advance();

        return TOKEN_CHAR;
    }

    /* bool */
    else if (currentToken.type == TOKEN_TRUE ||
             currentToken.type == TOKEN_FALSE) {
            expressionNode =
            createValueNode(
                "BOOL",
                currentToken.lexeme
            );

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


        /* guardar nodos */
        ASTNode *leftNode = expressionNode;

        ASTNode *rightNode;

        rightType = parseFactor();

        rightNode = expressionNode;

        /* operador */
        if (operatorType == TOKEN_MULTIPLY) {

            expressionNode =
                createOperatorNode(
                    "*",
                    leftNode,
                    rightNode
                );
        }

    else if (operatorType == TOKEN_DIVIDE) {

        /* division entre cero */

        if (rightNode != NULL &&
            strcmp(rightNode->value, "0") == 0) {

            semanticError(
                "Division entre cero",
                currentToken.line
            );
        }

        leftNode =
            createOperatorNode(
                "/",
                leftNode,
                rightNode
            );
    }
    else if (operatorType == TOKEN_MOD) {

        /* modulo entre cero */

        if (rightNode != NULL &&
            strcmp(rightNode->value, "0") == 0) {

            semanticError(
                "Modulo entre cero",
                currentToken.line
            );
        }

        leftNode =
            createOperatorNode(
                "%",
                leftNode,
                rightNode
            );
    }

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

        /* guardar nodo izquierdo */
        ASTNode *leftNode = expressionNode;

        advance();

        /* analizar lado derecho */
        rightType = parseTerm();

        /* guardar nodo derecho */
        ASTNode *rightNode = expressionNode;

        /* crear operador AST */

        if (operatorType == TOKEN_PLUS) {

            expressionNode =
                createOperatorNode(
                    "+",
                    leftNode,
                    rightNode
                );
        }

else if (operatorType == TOKEN_MINUS) {

    expressionNode =
        createOperatorNode(
            "-",
            leftNode,
            rightNode
        );
}

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
/* condicion */
void parseCondition() {

    TokenType leftType;
    TokenType rightType;
    TokenType relationalOperator;

/* negacion */

if (currentToken.type == TOKEN_NOT) {

    advance();

    /* !( condicion ) */

    if (currentToken.type == TOKEN_LPAREN) {

        advance();

        parseCondition();

        expect(TOKEN_RPAREN);

        return;
    }
}

    leftType = parseExpression();

    /* operador relacional */
    if (!isRelational(currentToken.type)) {
        syntaxError("Operador relacional esperado");
    }

    relationalOperator = currentToken.type;

    advance();

    rightType = parseExpression();

    /* comparacion produce bool */
    leftType = TOKEN_BOOL;
    rightType = TOKEN_BOOL;

    /* validacion semantica */

    /* bool solo con == y != */
/* PERO permitir resultados booleanos */

    if ((leftType == TOKEN_BOOL ||
        rightType == TOKEN_BOOL) &&

        relationalOperator != TOKEN_EQUAL &&
        relationalOperator != TOKEN_NOT_EQUAL) {

        /* permitir bool provenientes de comparaciones */

        if (!(leftType == TOKEN_BOOL &&
            rightType == TOKEN_BOOL)) {

            semanticError(
                "Comparacion invalida con bool",
                currentToken.line
            );
        }
    }

    /* string solo con == y != */
    if ((leftType == TOKEN_STRING ||
         rightType == TOKEN_STRING) &&

        relationalOperator != TOKEN_EQUAL &&
        relationalOperator != TOKEN_NOT_EQUAL) {

        semanticError(
            "Comparacion invalida con string",
            currentToken.line
        );
    }

    /* tipos incompatibles */

    if (leftType != rightType) {

        /* permitir int <-> float */

        if (!((leftType == TOKEN_INT &&
               rightType == TOKEN_FLOAT) ||

              (leftType == TOKEN_FLOAT &&
               rightType == TOKEN_INT))) {

            semanticError(
                "Tipos incompatibles en comparacion",
                currentToken.line
            );
        }
    }

    /* && y || */

/* operadores logicos */

while (currentToken.type == TOKEN_AND ||
       currentToken.type == TOKEN_OR) {

    advance();

    /* siguiente comparacion */

    leftType = parseExpression();

    if (!isRelational(currentToken.type)) {

        syntaxError(
            "Operador relacional esperado"
        );
    }

    advance();

    rightType = parseExpression();

    /* validar */

    if (!((leftType == TOKEN_INT ||
           leftType == TOKEN_FLOAT) &&

          (rightType == TOKEN_INT ||
           rightType == TOKEN_FLOAT))) {

        semanticError(
            "Operacion logica invalida",
            currentToken.line
        );
    }

    leftType = TOKEN_BOOL;
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
    expressionNode =
    createValueNode(
        "IDENTIFIER",
        variableName
    );

advance();

/* agregar simbolo */
addSymbol(variableName,
          declaredType,
          currentScope);

/* declaracion con inicializacion */
if (currentToken.type == TOKEN_ASSIGN) {

    advance();

    /* analizar expresion */
    expressionType = parseExpression();

    /* validar tipos */
    if (!areTypesCompatible(declaredType,
                            expressionType)) {

        semanticError(
            "Tipos incompatibles en declaracion",
            currentToken.line
        );
    }

    /* marcar inicializada */
    setInitialized(variableName,
                   currentScope);
}

    printf("Declaracion encontrada: %s\n",
        variableName);    

    /* AST declaracion */
    ASTNode *declarationNode =
        createNode(
            "DECLARATION",
            variableName
        );

    /* conectar expresion */
    declarationNode->left =
        expressionNode;

    /* agregar al AST */
    appendNode(&astRoot,
            declarationNode);
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

TokenType variableType;
TokenType expressionType;

/* tipo real variable */
variableType =
    getVariableType(
        variableName,
        currentScope
    );

/* analizar expresion */
expressionType =
    parseExpression();

/* validar tipos */
if (!areTypesCompatible(
        tokenTypeToDataType(variableType),
        expressionType)) {

    semanticError(
        "Tipos incompatibles en asignacion",
        currentToken.line
    );
}

setInitialized(variableName,
               currentScope);

    /* AST asignacion */
    ASTNode *assignmentNode =
        createNode(
            "ASSIGNMENT",
            variableName
        );

    /* conectar expresion */
    assignmentNode->left =
        expressionNode;

    /* agregar */
    appendNode(&astRoot,
            assignmentNode);
    expect(TOKEN_SEMICOLON);
}

/* print */
void parsePrint() {

    char printValue[100] = "expression";

    expect(TOKEN_PRINT);
    expect(TOKEN_LPAREN);

    /* string literal */
    if (currentToken.type == TOKEN_STRING_LITERAL) {

        strcpy(printValue,
               currentToken.lexeme);
    }

    parseExpression();

    /* AST print */
    ASTNode *printNode =
        createNode(
            "PRINT",
            printValue
        );

    /* conectar expresion */
    printNode->left =
        expressionNode;

    /* agregar */
    appendNode(&astRoot,
            printNode);

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

    char variableName[50];
    char declaredType[20];

    TokenType expressionType;

    expect(TOKEN_FOR);
    expect(TOKEN_LPAREN);

    /* tipo */

    if (!isType(currentToken.type)) {
        syntaxError(
            "Se esperaba tipo de dato en for"
        );
    }

    strcpy(
        declaredType,
        tokenTypeToDataType(currentToken.type)
    );

    advance();

    /* identificador */

    if (currentToken.type != TOKEN_IDENTIFIER) {
        syntaxError("Identificador esperado");
    }

    strcpy(variableName,
           currentToken.lexeme);

    advance();

    expect(TOKEN_ASSIGN);

    /* expresion inicial */

    expressionType = parseExpression();

    if (!areTypesCompatible(
            declaredType,
            expressionType)) {

        semanticError(
            "Tipos incompatibles en for",
            currentToken.line
        );
    }

    /* agregar variable */
    addSymbol(variableName,
              declaredType,
              currentScope);

    setInitialized(variableName,
                   currentScope);

    expect(TOKEN_SEMICOLON);

    /* condicion */
    parseCondition();

    expect(TOKEN_SEMICOLON);

    /* incremento */

    if (currentToken.type != TOKEN_IDENTIFIER) {
        syntaxError("Identificador esperado");
    }

    advance();

    expect(TOKEN_ASSIGN);

    parseExpression();

    expect(TOKEN_RPAREN);


    currentScope++;

    parseBlock();

    removeScopeSymbols(currentScope);

    currentScope--;
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
    /* eliminar variables del scope */
    removeScopeSymbols(currentScope);

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