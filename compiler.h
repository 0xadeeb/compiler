#ifndef COMPILER_H_
#define COMPILER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum types { number, OP, var, connector } types;

typedef struct symbolTable {
    char *name;
    int binding, sno;
    struct symbolTable *next;
} symbolTable;

typedef struct tnode {
    types t;
    union {
        int val;               // Number
        symbolTable *variable; // Var
        int op;                // Operator (+, -, read, write... etc)
    } u;
    struct tnode *left, *right;
    struct tnode *mid; // For if expressions
} tnode;

typedef struct registers {
    int i;
    struct registers *next;
} registers;

#define YYSTYPE tnode *

tnode *makeLeafNode(int);
tnode *makeOpNode(int, tnode *, tnode *);
int generateCode(tnode *);
tnode *lookup(char *name);
void printTree(tnode *);
tnode *connectNodes(tnode *left, tnode *right);
tnode *makeIfNode(int op, tnode *left, tnode *mid, tnode *right);

#endif // COMPILER_H_
