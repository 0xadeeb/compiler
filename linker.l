%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yywrap();

void Default(char c);
void findLabel(char *x);
void replaceLabel(char *x);

int pass = 1;
FILE *inFile, *outFile;
int lineNumber = 1;

typedef struct Label{
	char *key;
	int value;
	struct Label* next;
} lb ;

lb *head = NULL;

lb *create(char *label, int val){
	lb* node = malloc(sizeof(lb));
	node->key = label;
	node->value = val;
	node->next = NULL;
	return node;
}

void insert(char *label, int val){
	lb* node = create(label, val);
	node->next = head;
	head = node;
}

int getAddress(char *label){
	lb *l = head;
	while(l!=NULL){
		if (strcmp(l->key, label) == 0){
			return l->value;
		}
		l = l->next;
	}
}

%}

label [A-Z][0-9]*

%%

{label}":"        { findLabel(yytext);}
"|"{label}"|"     { replaceLabel(yytext); }
.                 { Default(*yytext); }
[\n]              { lineNumber++; Default(*yytext); }

%%

int yywrap(){

	if (pass == 1){
		pass++;
		outFile = fopen("output.xsm", "w");
		rewind(inFile);
		yyin = inFile;
		return 0;
	}

	fclose(inFile);
	fclose(outFile);
	
	return 1;
}

int main(){
	inFile = fopen("output.obj", "r");
	yyin = inFile;
	yylex();
	return 0;
}

void Default(char c){
	if(pass == 2){
		fprintf(outFile, "%c", c);
	}
}

void findLabel(char *x){
	if(pass == 1){
		int labelLength = strlen(x);
		char *label = malloc(sizeof(char) * (labelLength+1));
		strcpy(label, x);
		label[labelLength-1] = 0;
		insert(label, 2056+2*(lineNumber -9));
	}
}

void replaceLabel(char *x){
	if(pass == 2){
		int labelLength = strlen(x);
		char *label = malloc(sizeof(char) * labelLength);
		strcpy(label, x+1);
		label[labelLength-2] = 0;
		fprintf(outFile, "%d", getAddress(label));
	}
}
