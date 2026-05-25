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

    symbolCount++;
}

/* Verifica existencia */
int symbolExists(const char *name, int scope) {
    int i;

    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 &&
            symbolTable[i].scope <= scope) {
            return 1;
        }
    }

    return 0;
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