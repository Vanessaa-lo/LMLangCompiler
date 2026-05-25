#ifndef AST_H
#define AST_H

#define MAX_VALUE 100

typedef struct ASTNode {
    char type[50];
    char value[MAX_VALUE];

    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode* createNode(const char *type, const char *value);
void appendNode(ASTNode **root, ASTNode *newNode);
void printAST(ASTNode *node, int level);
void freeAST(ASTNode *node);

#endif