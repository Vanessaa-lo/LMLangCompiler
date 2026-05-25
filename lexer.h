#ifndef LEXER_H
#define LEXER_H

#define MAX_LEXEME 100

typedef enum {
    TOKEN_PROGRAM,
    TOKEN_END,

    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_BOOL,
    TOKEN_STRING,

    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ELSEIF,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_PRINT,

    TOKEN_TRUE,
    TOKEN_FALSE,

    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_FLOAT_NUMBER,
    TOKEN_CHAR_LITERAL,
    TOKEN_STRING_LITERAL,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MOD,

    TOKEN_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,

    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    TOKEN_SEMICOLON,
    TOKEN_COMMA,

    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_LBRACE,
    TOKEN_RBRACE,

    TOKEN_EOF,
    TOKEN_ERROR

} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME];
    int line;
} Token;

void initLexer(const char *filename);
Token getNextToken();
const char* tokenTypeToString(TokenType type);
void closeLexer();

#endif