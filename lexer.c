#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

FILE *sourceFile = NULL;
int currentLine = 1;

/* Inicializa el lexer abriendo el archivo fuente */
void initLexer(const char *filename) {
    sourceFile = fopen(filename, "r");

    if (!sourceFile) {
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        exit(1);
    }
}

/* Cierra el archivo fuente */
void closeLexer() {
    if (sourceFile) {
        fclose(sourceFile);
    }
}

/* Convierte el tipo de token a texto */
const char* tokenTypeToString(TokenType type) {
    switch(type) {
        case TOKEN_PROGRAM: return "TOKEN_PROGRAM";
        case TOKEN_END: return "TOKEN_END";

        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        case TOKEN_CHAR: return "TOKEN_CHAR";
        case TOKEN_BOOL: return "TOKEN_BOOL";
        case TOKEN_STRING: return "TOKEN_STRING";

        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_ELSEIF: return "TOKEN_ELSEIF";
        case TOKEN_FOR: return "TOKEN_FOR";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_DO: return "TOKEN_DO";
        case TOKEN_PRINT: return "TOKEN_PRINT";

        case TOKEN_TRUE: return "TOKEN_TRUE";
        case TOKEN_FALSE: return "TOKEN_FALSE";

        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_NUMBER: return "TOKEN_NUMBER";
        case TOKEN_FLOAT_NUMBER: return "TOKEN_FLOAT_NUMBER";
        case TOKEN_CHAR_LITERAL: return "TOKEN_CHAR_LITERAL";
        case TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";

        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_MULTIPLY: return "TOKEN_MULTIPLY";
        case TOKEN_DIVIDE: return "TOKEN_DIVIDE";
        case TOKEN_MOD: return "TOKEN_MOD";

        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_EQUAL: return "TOKEN_EQUAL";
        case TOKEN_NOT_EQUAL: return "TOKEN_NOT_EQUAL";

        case TOKEN_LESS: return "TOKEN_LESS";
        case TOKEN_GREATER: return "TOKEN_GREATER";
        case TOKEN_LESS_EQUAL: return "TOKEN_LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "TOKEN_GREATER_EQUAL";

        case TOKEN_AND: return "TOKEN_AND";
        case TOKEN_OR: return "TOKEN_OR";
        case TOKEN_NOT: return "TOKEN_NOT";

        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_COMMA: return "TOKEN_COMMA";

        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";

        case TOKEN_LBRACE: return "TOKEN_LBRACE";
        case TOKEN_RBRACE: return "TOKEN_RBRACE";

        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_ERROR: return "TOKEN_ERROR";

        default: return "UNKNOWN_TOKEN";
    }
}

/* Verifica si una palabra es reservada */
TokenType checkKeyword(const char *word) {
    if (strcmp(word, "program") == 0) return TOKEN_PROGRAM;
    if (strcmp(word, "end") == 0) return TOKEN_END;

    if (strcmp(word, "int") == 0) return TOKEN_INT;
    if (strcmp(word, "float") == 0) return TOKEN_FLOAT;
    if (strcmp(word, "char") == 0) return TOKEN_CHAR;
    if (strcmp(word, "bool") == 0) return TOKEN_BOOL;
    if (strcmp(word, "string") == 0) return TOKEN_STRING;

    if (strcmp(word, "if") == 0) return TOKEN_IF;
    if (strcmp(word, "else") == 0) return TOKEN_ELSE;
    if (strcmp(word, "elseif") == 0) return TOKEN_ELSEIF;
    if (strcmp(word, "for") == 0) return TOKEN_FOR;
    if (strcmp(word, "while") == 0) return TOKEN_WHILE;
    if (strcmp(word, "do") == 0) return TOKEN_DO;
    if (strcmp(word, "print") == 0) return TOKEN_PRINT;

    if (strcmp(word, "true") == 0) return TOKEN_TRUE;
    if (strcmp(word, "false") == 0) return TOKEN_FALSE;

    return TOKEN_IDENTIFIER;
}

/* Obtiene el siguiente token */
Token getNextToken() {
    Token token;
    token.line = currentLine;
    token.lexeme[0] = '\0';

    int ch;

    /* Saltar espacios y saltos de línea */
    while ((ch = fgetc(sourceFile)) != EOF) {
        if (ch == '\n') {
            currentLine++;
        }

        if (!isspace(ch)) {
            break;
        }
    }

    if (ch == EOF) {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    /* Identificadores o palabras reservadas */
    if (isalpha(ch) || ch == '_') {
        int i = 0;

        do {
            token.lexeme[i++] = ch;
            ch = fgetc(sourceFile);
        } while (isalnum(ch) || ch == '_');

        token.lexeme[i] = '\0';
        ungetc(ch, sourceFile);

        token.type = checkKeyword(token.lexeme);
        return token;
    }

/* Números */
if (isdigit(ch)) {

    int i = 0;
    int dotCount = 0;

    while (isdigit(ch) || ch == '.') {

        if (ch == '.') {

            dotCount++;

            /* Número inválido */
            if (dotCount > 1) {

                token.type = TOKEN_ERROR;

                strcpy(token.lexeme,
                       "Numero invalido");

                return token;
            }
        }

        token.lexeme[i++] = ch;

        ch = fgetc(sourceFile);
    }

    token.lexeme[i] = '\0';

    ungetc(ch, sourceFile);

    token.type = (dotCount == 1)
                 ? TOKEN_FLOAT_NUMBER
                 : TOKEN_NUMBER;

    return token;
}

    /* Strings */

    if (ch == '"') {

        int i = 0;

        while ((ch = fgetc(sourceFile)) != '"') {

            /* String sin cerrar */
            if (ch == EOF || ch == '\n') {

                token.type = TOKEN_ERROR;

                strcpy(token.lexeme,
                    "String sin cerrar");

                return token;
            }

            token.lexeme[i++] = ch;
        }

        token.lexeme[i] = '\0';

        token.type = TOKEN_STRING_LITERAL;

        return token;
    }

/* Chars */
if (ch == '\'') {

    int charValue = fgetc(sourceFile);

    /* Char sin cerrar */
    if (charValue == EOF ||
        charValue == '\n') {

        token.type = TOKEN_ERROR;

        strcpy(token.lexeme,
               "Char sin cerrar");

        return token;
    }

    int closingQuote = fgetc(sourceFile);

    /* Char vacío o múltiple */
    if (closingQuote != '\'') {

        token.type = TOKEN_ERROR;

        strcpy(token.lexeme,
               "Char invalido");

        return token;
    }

    token.lexeme[0] = charValue;
    token.lexeme[1] = '\0';

    token.type = TOKEN_CHAR_LITERAL;

    return token;
}

    /* Operadores y símbolos */
    switch(ch) {
        case '+':
            token.type = TOKEN_PLUS;
            strcpy(token.lexeme, "+");
            break;

        case '-':
            token.type = TOKEN_MINUS;
            strcpy(token.lexeme, "-");
            break;

        case '*':
            token.type = TOKEN_MULTIPLY;
            strcpy(token.lexeme, "*");
            break;

        case '/':

            ch = fgetc(sourceFile);

            /* Comentario de linea */
            if (ch == '/') {

                while ((ch = fgetc(sourceFile)) != '\n' &&
                    ch != EOF);

                if (ch == '\n') {
                    currentLine++;
                }

                return getNextToken();
            }

            /* Comentario multilinea */
            else if (ch == '*') {

                int prev = 0;

                while ((ch = fgetc(sourceFile)) != EOF) {

                    if (ch == '\n') {
                        currentLine++;
                    }

                    if (prev == '*' && ch == '/') {
                        break;
                    }

                    prev = ch;
                }

                /* Comentario sin cerrar */
                if (ch == EOF) {

                    token.type = TOKEN_ERROR;

                    strcpy(token.lexeme,
                        "Comentario sin cerrar");

                    return token;
                }

                return getNextToken();
            }

            /* Division normal */
            else {

                ungetc(ch, sourceFile);

                token.type = TOKEN_DIVIDE;

                strcpy(token.lexeme, "/");
            }

            break;

        case '%':
            token.type = TOKEN_MOD;
            strcpy(token.lexeme, "%");
            break;

        case ';':
            token.type = TOKEN_SEMICOLON;
            strcpy(token.lexeme, ";");
            break;

        case ',':
            token.type = TOKEN_COMMA;
            strcpy(token.lexeme, ",");
            break;

        case '(':
            token.type = TOKEN_LPAREN;
            strcpy(token.lexeme, "(");
            break;

        case ')':
            token.type = TOKEN_RPAREN;
            strcpy(token.lexeme, ")");
            break;

        case '{':
            token.type = TOKEN_LBRACE;
            strcpy(token.lexeme, "{");
            break;

        case '}':
            token.type = TOKEN_RBRACE;
            strcpy(token.lexeme, "}");
            break;

        case '=':
            ch = fgetc(sourceFile);
            if (ch == '=') {
                token.type = TOKEN_EQUAL;
                strcpy(token.lexeme, "==");
            } else {
                ungetc(ch, sourceFile);
                token.type = TOKEN_ASSIGN;
                strcpy(token.lexeme, "=");
            }
            break;

        case '!':
            ch = fgetc(sourceFile);
            if (ch == '=') {
                token.type = TOKEN_NOT_EQUAL;
                strcpy(token.lexeme, "!=");
            } else {
                ungetc(ch, sourceFile);
                token.type = TOKEN_NOT;
                strcpy(token.lexeme, "!");
            }
            break;

        case '<':
            ch = fgetc(sourceFile);
            if (ch == '=') {
                token.type = TOKEN_LESS_EQUAL;
                strcpy(token.lexeme, "<=");
            } else {
                ungetc(ch, sourceFile);
                token.type = TOKEN_LESS;
                strcpy(token.lexeme, "<");
            }
            break;

        case '>':
            ch = fgetc(sourceFile);
            if (ch == '=') {
                token.type = TOKEN_GREATER_EQUAL;
                strcpy(token.lexeme, ">=");
            } else {
                ungetc(ch, sourceFile);
                token.type = TOKEN_GREATER;
                strcpy(token.lexeme, ">");
            }
            break;

        case '&':
            if (fgetc(sourceFile) == '&') {
                token.type = TOKEN_AND;
                strcpy(token.lexeme, "&&");
            } else {
                token.type = TOKEN_ERROR;
            }
            break;

        case '|':
            if (fgetc(sourceFile) == '|') {
                token.type = TOKEN_OR;
                strcpy(token.lexeme, "||");
            } else {
                token.type = TOKEN_ERROR;
            }
            break;

            default:

            token.type = TOKEN_ERROR;

            sprintf(token.lexeme,
                    "Caracter invalido: %c",
                    ch);

            break;
    }

    return token;
}