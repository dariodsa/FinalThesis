%{
#include <stdio.h>

enum {
  LOOKUP = 0,
  VERB,
  ADJ,
  ADV,
  NOUN
};

int state;
int add_word(int type, char *word);
int lookup_word(char *word);

%}
%%

[\t ]+ /*ignore whitespace */ ;
is |
am |
where |
was |
be |
being |
been |
do |
does |
did |
will |
should |
would |
can |
could |
has |
have |
had |
go { printf("%s: is a verb\n", yytext); }


very |
simple |
gently |
quietly |
angrily |
calmly { printf("%s is an adverb\n", yytext); }

[a-zA-Z]+ {printf("%s is not verb\n", yytext); }
.|\n { state = LOOKUP;}
%%


main() 
{
 yylex();
}
