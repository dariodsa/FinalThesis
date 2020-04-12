
%{
#include "y.tab.h"
#include <string.h>

%}


%option caseless


%%


SELECT { return SELECT; }
UNION  { return UNION; }
FROM   { return FROM; }
WHERE  { return WHERE; }
LIMIT  { return LIMIT; }
OFFSET { return OFFSET; }
HAVING { return HAVING; }
ONLY { return ONLY; }
BY     { return BY; }
GROUP  { return GROUP; }
ORDER  { return ORDER; }
CROSS { return CROSS; }

DISTINCT { return DISTINCT; }

AS { return AS; }
END { return END;}
WHEN { return WHEN; }

ALTER { return ALTER; }

DISTINCT { return DISTINCT;}
ALL { return ALL;}
ANY  { return ANY; }
SOME { return SOME; }

JOIN { return JOIN;}
NATURAL { return NATURAL; }
LEFT { return LEFT; }
RIGHT { return RIGHT; }
INNER { return INNER; }
FULL  { return FULL; }
OUTER { return OUTER;}
ON { return ON;}
USING { return USING; }

CHECK { return CHECK; }
UPDATE { return UPDATE; }
DELETE { return DELETE; }
SET { return SET; }
DEFAULT { return DEFAULT; }
CASCADE { return CASCADE; }
REFERENCES { return REFERENCES;}

ADD { return ADD; }

NOT { return NOT; }
AND { return AND; }
OR { return OR; }

"<"  |
"<=" |
">"  |
">=" |
"="  |
"<>"    { yylval.text =strdup(yytext); return CMP; }

array_agg |
bit_and |
bit_or  |
bool_or |
bool_and |
date |
every |
json_agg |
jsonb_agg |
interval |
xmlagg {return SINGLE_AGG_FUNCTION;}




BETWEEN { return BETWEEN; }
NULL  { return NULL_STR; }
IN { return IN; }
IS { return IS; }
LIKE { return LIKE; }
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

[-+*/:(),.;'"] { return yytext[0]; }

\"[^\"]*[\"] { return QUOTED_STRING;}
\'[^\']*[\'] { return SINGLE_QUOTED_STRING;}

[A-Za-z][A-Za-z0-9_]* { yylval.text =strdup(yytext); return NAME; }

[0-9]+           |
[0-9]+"."[0-9]* |
"."[0-9]* { yylval.number=atof(yytext); return NUMBER; }

[0-9]+[eE][+-]?[0-9]+          |
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+       { return ENUMBER; }

[ \t\r]+  ; /*white space*/
"--".*    ; /*comment*/



\n { printf("new line\n");lineno++; }

%%

