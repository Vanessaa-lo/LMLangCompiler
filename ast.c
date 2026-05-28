#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* crear nodo */
ASTNode* createNode(const char *type, const char *value) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));

    if (node == NULL) {
        printf("Error: No se pudo asignar memoria para AST.\n");
        exit(1);
    }

    strcpy(node->type, type);
    strcpy(node->value, value);

    node->left = NULL;
    node->right = NULL;
    node->next = NULL;

    return node;
}

void appendNode(ASTNode **root, ASTNode *newNode) {
    ASTNode *current;

    if (*root == NULL) {
        *root = newNode;
        return;
    }

    current = *root;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = newNode;

    printf("Nodo agregado: %s (%s)\n",
       newNode->type,
       newNode->value);
}

/* imprimir AST */
void printAST(ASTNode *node, int level) {

    int i;

    while (node != NULL) {

        /* indentacion */
        for (i = 0; i < level; i++) {
            printf("   ");
        }

        /* nodo */
        printf("%s", node->type);

        if (strlen(node->value) > 0) {
            printf(" (%s)", node->value);
        }

        printf("\n");

        /* hijos */
        printAST(node->left, level + 1);

        printAST(node->right, level + 1);

        /* siguiente sentencia */
        node = node->next;
    }
}
/* liberar memoria */
void freeAST(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    freeAST(node->left);
    freeAST(node->right);

    free(node);
}

/* crear nodo operador */
ASTNode* createOperatorNode(
    const char *operator,
    ASTNode *left,
    ASTNode *right) {

    ASTNode *node =
        createNode("OPERATOR", operator);

    node->left = left;
    node->right = right;

    return node;
}

/* crear nodo valor */
ASTNode* createValueNode(
    const char *type,
    const char *value) {

    return createNode(type, value);
}