#include "compiler.h"
#include "parser.tab.h"

registers *head = NULL, *tail;
symbolTable *shead = NULL, *stail;

tnode *makeLeafNode(int n) {
    tnode *node = (tnode *)malloc(sizeof(tnode));
    node->t = number;
    node->u.val = n;
    node->left = NULL;
    node->right = NULL;
    return node;
}

tnode *makeOpNode(int op, tnode *left, tnode *right) {
    tnode *node = (tnode *)malloc(sizeof(tnode));
    node->t = OP;
    node->u.op = op;
    node->left = left;
    node->right = right;
    return node;
}

tnode *makeIfNode(int op, tnode *left, tnode *mid, tnode *right) {
    tnode *node = (tnode *)malloc(sizeof(tnode));
    node->t = OP;
    node->u.op = op;
    node->left = left;
    node->mid = mid;
    node->right = right;
    return node;
}

void allocRegs() {
    registers *t, *p;
    for (int i = 0; i < 20; i++) {
        t = (registers *)malloc(sizeof(registers));
        t->i = i;
        if (!head)
            head = t;
        else
            p->next = t;
        p = t;
    }
    tail = t;
}

int getReg() {
    if (head == NULL) {
        printf("No more registers available\n");
        exit(1);
    }
    int reg = head->i;
    registers *t = head;
    head = head->next;
    free(t);
    return reg;
}

void freeReg(int i) {
    registers *t = (registers *)malloc(sizeof(registers));
    t->i = i;
    if (head == NULL) {
        head = t;
        tail = t;
    } else {
        tail->next = t;
        tail = t;
    }
}

int getAddress(tnode *node) {
    int address = node->u.variable->binding;
    return address;
}

int getLabel() {
    static int label = 0;
    return label++;
}

int genCode(tnode *tree, FILE *fp) {
    if (tree) {
        int reg1 = -1, reg2, l1, l2;
        switch (tree->t) {
        case number:
            reg1 = getReg();
            fprintf(fp, "MOV R%d, %d\n", reg1, tree->u.val);
            return reg1;
        case var:
            reg1 = getReg();
            reg2 = getReg();
            fprintf(fp, "MOV R%d, %d\n", reg1, getAddress(tree));
            fprintf(fp, "MOV R%d, [R%d]\n", reg2, reg1);
            freeReg(reg1);
            return reg2;
        case OP:
            switch (tree->u.op) {
            case MULT:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "MUL R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case SUB:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "SUB R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case ADD:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "ADD R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case EQ:
                reg1 = getReg();
                fprintf(fp, "MOV R%d, %d\n", reg1, getAddress(tree->left));
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "MOV [R%d], R%d\n", reg1, reg2);
                freeReg(reg1);
                freeReg(reg2);
                return -1;
            case GE:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "GE R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case LE:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "LE R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case LT:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "LT R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case GT:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "GT R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case EE:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "EQ R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case NE:
                reg1 = genCode(tree->left, fp);
                reg2 = genCode(tree->right, fp);
                fprintf(fp, "NE R%d, R%d\n", reg1, reg2);
                freeReg(reg2);
                return reg1;
            case IF:
                l1 = getLabel();
                l2 = getLabel();
                reg1 = genCode(tree->mid, fp);
                fprintf(fp, "JZ R%d, |L%d|\n", reg1, l1);
                genCode(tree->left, fp);
                fprintf(fp, "JMP |L%d|\n", l2);
                fprintf(fp, "L%d:", l1);
                genCode(tree->right, fp);
                fprintf(fp, "L%d:", l2);
                freeReg(reg1);
                return -1;
            case WHILE:
                l1 = getLabel();
                fprintf(fp, "L%d:", l1);
                genCode(tree->left, fp);
                reg1 = genCode(tree->right, fp);
                fprintf(fp, "JNZ R%d, |L%d|\n", reg1, l1);
                freeReg(reg1);
                return -1;
            case READ:
                reg1 = getReg();
                fprintf(fp, "MOV R%d, 7\n", reg1);                          // Move val
                fprintf(fp, "PUSH R%d\n", reg1);                            // system call number
                fprintf(fp, "MOV R%d, -1\n", reg1);                         // Move val
                fprintf(fp, "PUSH R%d\n", reg1);                            // Arg 1
                fprintf(fp, "MOV R%d, %d\n", reg1, getAddress(tree->left)); // Move address
                fprintf(fp, "PUSH R%d\n", reg1); // Arg 2 (address to read to)
                fprintf(fp, "PUSH R%d\n", reg1); // Arg 3
                fprintf(fp, "PUSH R%d\n", reg1); // Return Value
                fprintf(fp, "INT 6\n");          // Intrupt call
                fprintf(fp, "POP R%d\n", reg1);  // Pop return value
                fprintf(fp, "POP R%d\n", reg1);  // Pop Arg 3
                fprintf(fp, "POP R%d\n", reg1);  // Pop Arg 2
                fprintf(fp, "POP R%d\n", reg1);  // Pop Arg 1
                fprintf(fp, "POP R%d\n", reg1);  // Pop system call number
                freeReg(reg1);
                return -1;
            case WRITE:
                reg1 = getReg();
                fprintf(fp, "MOV R%d, 5\n", reg1);
                fprintf(fp, "PUSH R%d\n", reg1);
                fprintf(fp, "MOV R%d, -2\n", reg1);
                fprintf(fp, "PUSH R%d\n", reg1);
                fprintf(fp, "MOV R%d, R%d\n", reg1, genCode(tree->left, fp));
                fprintf(fp, "PUSH R%d\n", reg1);
                fprintf(fp, "PUSH R%d\n", reg1);
                fprintf(fp, "PUSH R%d\n", reg1);
                fprintf(fp, "INT 7\n");
                fprintf(fp, "POP R%d\n", reg1);
                fprintf(fp, "POP R%d\n", reg1);
                fprintf(fp, "POP R%d\n", reg1);
                fprintf(fp, "POP R%d\n", reg1);
                fprintf(fp, "POP R%d\n", reg1);
                freeReg(reg1);
                return -1;
            }
        case connector:
            genCode(tree->left, fp);
            genCode(tree->right, fp);
            return -1;
        }
        return reg1;
    }
}

tnode *connectNodes(tnode *left, tnode *right) {
    tnode *node = (tnode *)malloc(sizeof(tnode));
    node->left = left;
    node->right = right;
    node->t = connector;
    return node;
}

void printTree(tnode *root) {
    if (root) {
        printf("( ");
        printTree(root->left);
        if (root->t == OP)
            printf("'%d' ", root->u.op);
        else
            printf("%d ", root->u.val);
        printTree(root->right);
        printf(" )");
    }
}

tnode *insertSymbol(char *name) {
    symbolTable *t;
    t = (symbolTable *)malloc(sizeof(symbolTable));
    t->name = (char *)malloc(strlen(name));
    strcpy(t->name, name);
    t->next = NULL;
    if (shead == NULL) {
        t->binding = 4096;
        t->sno = 1;
        shead = t;
        stail = t;
    } else {
        t->binding = stail->binding + 1;
        t->sno = stail->sno + 1;
        stail->next = t;
        stail = t;
    }

    tnode *node = (tnode *)malloc(sizeof(tnode));
    node->t = var;
    node->u.variable = t;
    node->left = NULL;
    node->right = NULL;
    return node;
}

tnode *lookup(char *name) {
    symbolTable *t = shead;
    while (t) {
        // printf("Var name = %s\n", t->name);
        if (strcmp(name, t->name) == 0) {
            tnode *node = (tnode *)malloc(sizeof(tnode));
            node->t = var;
            node->u.variable = t;
            node->left = NULL;
            node->right = NULL;
            return node;
        }
        t = t->next;
    }
    return insertSymbol(name);
}

int generateCode(tnode *root) {
    FILE *fp = fopen("output.obj", "w");
    fprintf(fp, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", 0, 2056, 0, 0, 0, 0, 0, 0);
    fprintf(fp, "MOV SP, %d\n", 4096 + stail->sno);
    allocRegs();
    int reg;
    reg = genCode(root, fp);
    fprintf(fp, "MOV R0, 10\n");
    for (int i = 0; i < 5; i++) {
        fprintf(fp, "PUSH R0\n");
    }
    fprintf(fp, "INT 10\n");
    return reg;
}
