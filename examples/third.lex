%{
#include <stdio.h>
%}
%%
[a-zA-Z][a-zA-Z0-9]* printf("WORD");
[a-zA-Z0-9\/.-]+ printf("FILENAME");
\"               printf("QUOTE");
\}               printf("BEGIN BRACE");
\{               printf("END BRACE");
;                printf("SEMICOLON");
\n               printf("NEWLINE");
[ \t]+           /*ignore whitespaces*/
%%
