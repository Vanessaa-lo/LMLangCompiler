#ifndef AST_H
#define AST_H

#define MAX_VALUE 100

typedef struct ASTNode {

    char type[50];
    char value[MAX_VALUE];

    /* hijos del arbol */
    struct ASTNode *left;
    struct ASTNode *right;

    /* siguiente sentencia */
    struct ASTNode *next;

} ASTNode;

ASTNode* createNode(const char *type, const char *value);
void appendNode(ASTNode **root, ASTNode *newNode);
void printAST(ASTNode *node, int level);
void freeAST(ASTNode *node);

ASTNode* createOperatorNode(
    const char *operator,
    ASTNode *left,
    ASTNode *right
);

ASTNode* createValueNode(
    const char *type,
    const char *value
);

#endif