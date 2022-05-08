%{
#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
int yyerror(char*);
extern FILE *yyin;
int count = 0;
%}

%token NUM
%token EQ
%token FUN
%token IF
%token ELSE
%token DO
%token WHILE
%token READ
%token WRITE
%token VAR
%token RETURN
%token ARGC
%nonassoc GE LE EE NE GT LT
%left ADD SUB
%left MULT

%%

Program:        FUN '(' ')' '{' Stmts '}' { generateCode($5); }
        ;

Stmts:          Stmt Stmts {$$ = connectNodes($1, $2);}
        |       Stmt {$$ = $1;}
        ;

Stmt:           AsgStmt {$$ = $1;}
        |       IfStmt {$$ = $1;}
        |       IfElseStmt {$$ = $1;}
        |       WhileStmt {$$ = $1;}
        |       ReadStmt {$$ = $1;}
        |       WriteStmt {$$ = $1;}
        ;

AsgStmt:        VAR EQ E ';'  {$$ = makeOpNode(EQ, $1, $3);}
        ;

IfStmt:         IF '(' E ')' '{' Stmts '}' {$$ = makeIfNode(IF, $6, $3, NULL);}
        ;

IfElseStmt:     IF '(' E ')' '{' Stmts '}' ELSE '{' Stmts '}' {$$ = makeIfNode(IF, $6, $3, $10);}
        ;

WhileStmt:      DO '{' Stmts '}' WHILE '(' E ')' ';' { $$ = makeOpNode(WHILE, $3, $7); }
        ;

ReadStmt:       READ '(' VAR ')' ';'
                {
                    $$ = makeOpNode(READ, $3, NULL);
                }
        ;

WriteStmt:      WRITE '(' E ')' ';'
                {
                    $$ = makeOpNode(WRITE, $3, NULL);
                }
        ;

E:              E MULT E {$$ = makeOpNode(MULT, $1, $3);}
        |       E ADD E {$$ = makeOpNode(ADD, $1, $3);}
        |       E SUB E {$$ = makeOpNode(SUB, $1, $3);}
        |       E GE E {$$ = makeOpNode(GE, $1, $3);}
        |       E GT E {$$ = makeOpNode(GT, $1, $3);}
        |       E LT E {$$ = makeOpNode(LT, $1, $3);}
        |       E LE E {$$ = makeOpNode(LE, $1, $3);}
        |       E EE E {$$ = makeOpNode(EE, $1, $3);}
        |       E NE E {$$ = makeOpNode(NE, $1, $3);}
        |       '(' E ')' {$$ = $2;}
        |       VAR {$$ = $1;}
        |       NUM {$$ = $1;}
        ;

%%

int yyerror(char *s) {
        printf("Error: %s\n", s);
        return 0;
}

int main(int argc, char** argv) {
        yyin = fopen(argv[1], "r");
        yyparse();
        return 0;
 }
