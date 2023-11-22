// global.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BSIZE 128
#define NONE  -1
#define EOS   '\0'

#define NUM   256
#define DIV   257
#define MOD   258
#define ID    259
#define DONE  260

int tokenval;
int lineno;

struct entry {
	char *lexptr;
	int token;
};

struct entry symtable[];

int lexan();
void parse();
void parse();
void expr();
void term();
void factor();
void match(int);
void emit(int, int);
int lookup(char[]);
int insert(char[], int);
void init();
void error(char*);


// lexer.c
char lexbuf[BSIZE];
int lineno = 1;
int tokenval = NONE;

int lexan()
{
	int t;
	while(1) {
		t = getchar();
		if (t == ' ' || t == '\t')
			;
		else if ( t == '\n')
			lineno = lineno + 1;
		else if (isdigit(t)) {
			ungetc(t, stdin);
			scanf("%d", &tokenval);    // for gcc
			//scanf_s("%d", &tokenval);    // for Visual Studio
			return NUM;
		}
		else if (isalpha(t)) {
			int p,b = 0;
			while (isalnum(t)) {
				lexbuf[b] = t;
				t = getchar();
				b = b + 1;
				if (b >= BSIZE)
					error("compiler error");
			}
			lexbuf[b] = EOS;
			if (t !=EOF)
				ungetc(t, stdin);
			p = lookup(lexbuf);
			if (p == 0)
				p = insert(lexbuf, ID);
			tokenval = p;
			return symtable[p].token;
		}
		else if (t == EOF)
			return DONE;
		else {
			tokenval = NONE;
			return t;
		}
	}
}


// parser.c
int lookahead;

void parse()
{
	lookahead = lexan();
	while (lookahead != DONE) {
		expr(); match(';');
	}
}

void expr()
{
	int t;
	term();
	while(1)
		switch (lookahead) {
		case '+': case '-':
			t = lookahead;
			match(lookahead); term(); emit(t, NONE);
			continue;
		default:
			return;
		}
}

void term()
{
	int t;
	factor();
	while(1)
		switch (lookahead) {
		case '*': case '/': case DIV: case MOD:
			t = lookahead;
			match(lookahead); factor(); emit(t, NONE);
			continue;
		default:
			return;
		}
}

void factor()
{
	switch(lookahead) {
		case '(':
			match('('); expr(); match(')'); break;
		case NUM:
			emit(NUM, tokenval); match(NUM); break;
		case ID:
			emit(ID, tokenval); match(ID); break;
		default:
			error("syntax error"); break;
	}
}

void match(t)
	int t;
{
	if (lookahead == t)
		lookahead = lexan();
	else error("syntax error");
}


// emitter.c
void emit(t, tval)
	int t, tval;
{
	switch(t) {
		case '+': case '-': case '*': case '/':
			printf("%c\n", t); break;
		case DIV:
			printf("DIV\n"); break;
		case MOD:
			printf("MOD\n"); break;
		case NUM:
			printf("%d\n", tval); break;
		case ID:
			printf("%s\n", symtable[tval].lexptr); break;
		default:
			printf("token %d, tokenval %d\n", t, tval); break;
	}
}


// symbol.c
#define STRMAX	999
#define SYMMAX	100

char lexemes[STRMAX];
int lastchar = -1;
struct entry symtable[SYMMAX];
int lastentry = 0;

int lookup(s)
	char s[];
{
	int p;
	for (p = lastentry; p > 0; p = p -1)
		if (strcmp(symtable[p].lexptr, s) == 0)
			return p;
	return 0;
}

int insert(s, tok)
	char s[];
	int tok;
{
	int len;
	len = strlen(s);
	if (lastentry +1 >= SYMMAX)
		error("symbol table full");
	if (lastchar + len +1 >= STRMAX)
		error("lexemes array full");
	lastentry = lastentry +1;
	symtable[lastentry].token = tok;
	symtable[lastentry].lexptr = &lexemes[lastchar + 1];
	lastchar = lastchar + len + 1;
	strcpy(symtable[lastentry].lexptr, s);
	return lastentry;
}


// init.c
struct entry keywords[] = {
	"div", DIV,
	"mod", MOD,
	0, 0
};

void init()
{
	struct entry *p;
	for (p = keywords; p -> token; p++)
		insert(p -> lexptr, p -> token);
}


// error.c
void error(m)
	char *m;
{
	fprintf(stderr, "line %d: %s\n", lineno, m);
	exit(1);
}


// main.c
void main()
{
	init();
	parse();
	exit(0);
}
