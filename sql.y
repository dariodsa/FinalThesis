%{
#include "const.h"
#include <stdio.h>
%}

%token SELECT FROM STRING

%token NAME
%token STRING
%token NUMBER

%%

%%


void yyerror(char *s) {
    printf("%d: %s at %s\n", lineno, s, yytext);
}

int main(int argc, char* argv[]) {
    if(argc > 1 && (yyin = fopen(argv[1], "r")) == NULL) {
      perror(argv[1]);
      exit(1);
    }
    printf("%d\n", yylex);
    printf(yylex()?"SQL parse failed.\n":"SQL parse success.\n");
  
