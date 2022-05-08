# Minimal compiler

## Grammar
Keywords : FUN, if, else, do, while, read, write

Program ::= FUN() { Stmts } /* There is only one function that returns an integer */

Stmts ::= Stmts Stmt
	| Stmt

Stmt ::= AsgStmt
	| IfStmt
	| IfElseStmt
	| WhileStmt
	| ReadStmt
	| WriteStmt

AsgStmt ::= VAR = E;

IfStmt ::= If (E) { Stmts }

IfElseStmt ::= If (E) { Stmts } else { Stmts }

WhileStmt ::= do { Stmts } while (E);

ReadStmt ::= read(VAR); /* Reads an integer into a variable */

WriteStmt ::= write(E); /* Prints an integer expression */

E ::= E + E
	| E - E
	| E * E
	| E <= E
	| E >= E
	| E == E
	| E != E
	| (E)
	| NUM
	| VAR /* All expressions evaluate to integer values, as in C */

NUM :== digit(digit)∗

VAR :== letter(letter|digit)∗
/* Variables can store only integers */
