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

    return node;
}

void appendNode(ASTNode **root, ASTNode *newNode) {
    ASTNode *current;

    if (*root == NULL) {
        *root = newNode;
        return;
    }

    current = *root;

    while (current->right != NULL) {
        current = current->right;
    }

    current->right = newNode;
}

/* imprimir AST */
void printAST(ASTNode *node, int level) {
    int i;

    if (node == NULL) {
        return;
    }

    for (i = 0; i < level; i++) {
        printf("   ");
    }

    printf("%s", node->type);

    if (strlen(node->value) > 0) {
        printf(" (%s)", node->value);
    }

    printf("\n");

    printAST(node->left, level + 1);
    printAST(node->right, level + 1);
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