%{
#include <stdio.h>
#include <string.h>
int lineno = 1;
%}

%token SELECT UNION FROM WHERE LIMIT OFFSET HAVING BY GROUP ORDER JOIN NATURAL LEFT RIGHT INNER FULL OUTER ON USING NOT AND OR CMP BETWEEN NULL_STR IN EXISTS CASE THEN ELSE

%union
{
   double number;
   char *text;
}


%%

sts:
   | sts st;

st: NAME','NUMBER { printf("Imam name (%s) i number %.2f.\n", $1.text, $3.number);};
%%


void yyerror(char *s) {
    printf("%d: %s at \n", lineno, s);
}

int main(int argc, char* argv[]) {
   yyparse();
 }
