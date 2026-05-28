#ifndef SYMBOLS_H
#define SYMBOLS_H
#include "lexer.h"
#define MAX_SYMBOLS 100
#define MAX_NAME 50
#define MAX_TYPE 20

typedef struct {
    char name[MAX_NAME];
    char type[MAX_TYPE];

    int scope;

    /* inicializacion */
    int initialized;

} Symbol;

void initSymbolTable();
void addSymbol(const char *name, const char *type, int scope);
int symbolExists(const char *name, int scope);
Symbol* getSymbol(const char *name, int scope);
void printSymbolTable();
int symbolExistsInCurrentScope(const char *name, int scope);
TokenType getVariableType(const char *name, int scope);
void setInitialized(const char *name, int scope);
int isInitialized(const char *name, int scope);
void removeScopeSymbols(int scope);

#endif