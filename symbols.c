#include <stdio.h>
#include <string.h>
#include "symbols.h"

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

/* Inicializa tabla */
void initSymbolTable() {
    symbolCount = 0;
}

/* Agregar símbolo */
void addSymbol(const char *name, const char *type, int scope) {
    if (symbolCount >= MAX_SYMBOLS) {
        printf("Error: Tabla de simbolos llena.\n");
        return;
    }

    strcpy(symbolTable[symbolCount].name, name);
    strcpy(symbolTable[symbolCount].type, type);
    symbolTable[symbolCount].scope = scope;
    symbolTable[symbolCount].initialized = 0;

    symbolCount++;
}

/* verificar existencia */
int symbolExists(const char *name, int scope) {

    int i;
    int bestScope = -1;

    for (i = 0; i < symbolCount; i++) {

        if (strcmp(symbolTable[i].name,
                   name) == 0) {

            /* scope valido */
            if (symbolTable[i].scope <= scope) {

                if (symbolTable[i].scope >
                    bestScope) {

                    bestScope =
                        symbolTable[i].scope;
                }
            }
        }
    }

    return bestScope != -1;
}

int symbolExistsInCurrentScope(const char *name, int scope) {
    int i;

    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 &&
            symbolTable[i].scope == scope) {
            return 1;
        }
    }

    return 0;
}

/* Obtener símbolo */
Symbol* getSymbol(const char *name, int scope) {
    int i;

    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 &&
            symbolTable[i].scope == scope) {
            return &symbolTable[i];
        }
    }

    return NULL;
}

/* Mostrar tabla */
void printSymbolTable() {
    int i;

    printf("\n=== TABLA DE SIMBOLOS ===\n");
    printf("%-15s %-10s %-10s\n", "Nombre", "Tipo", "Scope");

    for (i = 0; i < symbolCount; i++) {
        printf("%-15s %-10s %-10d\n",
               symbolTable[i].name,
               symbolTable[i].type,
               symbolTable[i].scope);
    }
}

/* obtener tipo de variable */
/* obtener tipo de variable */
TokenType getVariableType(const char *name,
                          int scope) {

    int i;
    int bestScope = -1;

    for (i = 0; i < symbolCount; i++) {

        if (strcmp(symbolTable[i].name, name) == 0 &&
            symbolTable[i].scope <= scope) {

            /* tomar scope mas cercano */
            if (symbolTable[i].scope > bestScope) {

                bestScope = symbolTable[i].scope;

                if (strcmp(symbolTable[i].type, "int") == 0)
                    return TOKEN_INT;

                if (strcmp(symbolTable[i].type, "float") == 0)
                    return TOKEN_FLOAT;

                if (strcmp(symbolTable[i].type, "char") == 0)
                    return TOKEN_CHAR;

                if (strcmp(symbolTable[i].type, "bool") == 0)
                    return TOKEN_BOOL;

                if (strcmp(symbolTable[i].type, "string") == 0)
                    return TOKEN_STRING;
            }
        }
    }

    return TOKEN_ERROR;
}

/* marcar inicializada */
void setInitialized(const char *name, int scope) {

    int i;

    for (i = symbolCount - 1; i >= 0; i--) {

        if (strcmp(symbolTable[i].name, name) == 0 &&
            symbolTable[i].scope <= scope) {

            symbolTable[i].initialized = 1;

            return;
        }
    }
}

/* verificar inicializacion */
int isInitialized(const char *name, int scope) {

    int i;
    int bestScope = -1;
    int initialized = 0;

    for (i = 0; i < symbolCount; i++) {

        if (strcmp(symbolTable[i].name, name) == 0 &&
            symbolTable[i].scope <= scope) {

            /* tomar el scope mas cercano */
            if (symbolTable[i].scope > bestScope) {

                bestScope = symbolTable[i].scope;

                initialized =
                    symbolTable[i].initialized;
            }
        }
    }

    return initialized;
} 

/* eliminar simbolos del scope */
void removeScopeSymbols(int scope) {

    int i;
    int j;

    for (i = 0; i < symbolCount; i++) {

        if (symbolTable[i].scope == scope) {

            /* mover elementos */
            for (j = i; j < symbolCount - 1; j++) {

                symbolTable[j] =
                    symbolTable[j + 1];
            }

            symbolCount--;

            i--;
        }
    }
}
        