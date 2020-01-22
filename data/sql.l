
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
BY     { return BY; }
GROUP  { return GROUP; }
ORDER  { return ORDER; }
CROSS { return CROSS; }


AS { return AS; }
END { return END;}
WHEN { return WHEN; }

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

array_agg |
avg |
bit_and |
bit_or |
bool_and |
bool_or | 
count |
every |
json_agg |
jsonb_agg |
json_object_agg |
jsonb_object_agg |
max |
min |
string_agg |
sum |
xmlagg  { return AGG_FUNCTION; }

"<"  |
"<=" |
">"  |
">=" |
"="  |
"<>"    { return CMP; }

bigint |
int8   |
bigserial |
serial8 |
boolean |
integer |
int     |
int4    |
real    | 
float4  |
float8  |
json    |
xml     |
smallint |
int2   |
text    { return DATA_TYPE; }


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

[-+*/:(),.;] { return yytext[0]; }

[A-Za-z][A-Za-z0-9_]* { yylval.text =strdup(yytext); printf("strlen %d\n", strlen(yytext));return NAME; }

[0-9]+           |
[0-9]+"."[0-9]* |
"."[0-9]* { yylval.number=atof(yytext); printf("num %lf\n", yylval.number);return NUMBER; }

[0-9]+[eE][+-]?[0-9]+          |
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+       { return ENUMBER; }

[ \t\r]+  ; /*white space*/
"--".*    ; /*comment*/

'[^'\n]*'  {
      int c = yyinput();
      unput(c);
      if(c != '\'')
            return STRING;
      else yymore();
}


%%
