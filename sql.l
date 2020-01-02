
%{
#include <string.h>
#include "const.h"
int lineno = 1;
void yyerror(char *s);
%}

%e 1200
%%

SELECT { return SELECT; }
UNION  { return UNION; }
FROM   { return FROM; }
WHERE  { return WHERE; }
LIMIT  { return LIMIT; }
OFFSET { return OFFSET; }
HAVING { return HAVING; }
BY     { return BY; }
GROUP  { return GROUP; }
ORDER  { return ORDER; }

JOIN { return JOIN;}
NATURAL { return NATURAL; }
LEFT { return LEFT; }
RIGHT { return RIGHT; }
INNER { return INNER; }
FULL  { return FULL; }
OUTER { return OUTER;}
ON { return ON;}
USING { return USING; }

NOT { return NOT; }
AND { return AND; }
OR { return OR; }

"<"  |
"<=" |
">"  | 
">=" |
"="  | 
"<>"    { return CMP; }

BETWEEN { return BETWEEN; }
NULL  { return NULL_STR; }
IN { return IN; }
EXISTS { return EXISTS; }
CASE { return CASE; }
THEN { return THEN; }
ELSE { return ELSE; }

VALUES { return VALUES; }
INSERT { return INSERT; }
INTO { return INTO; }

CREATE { return CREATE; }
TABLE { return TABLE; }

UNIQUE { return UNIQUE; }
PRIMARY { return PRIMARY; }
FOREIGN { return FOREIGN; }
KEY { return KEY; }
CONSTRAINT { return CONSTRAINT; }

INDEX { return INDEX; }
ASC { return ASC; }
DESC { return DESC; }

[-+*/:(),.;] { return yytext[0]; }

[A-Za-z][A-Za-z0-9_]* { return NAME; }

[0-9]+           |
[0-9]+"."[0-9]* |
"."[0-9]+ { return NUMBER; }

[0-9]+[eE][+-]?[0-9]+          |
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+       { return ENUMBER; }

\n   lineno++;
[ \t\r]+  ; /*white space*/
"--".*    ; /*comment*/

'[^'\n]*'  {
      int c = input();
      unput(c);
      if(c != '\'')
            return STRING;
      else yymore();
}
'[^'\n]*$ { yyerror("Unterminated string");}

. yyerror("invalid character");

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
}
