%{
#include <stdio.h>
#include <string.h>
int lineno = 1;
int yylex();
%}

%token SELECT UNION DISTINCT ALL FROM WHERE LIMIT OFFSET HAVING BY GROUP ORDER JOIN NATURAL LEFT RIGHT INNER FULL OUTER ON USING NOT AND OR CMP BETWEEN NULL_STR IN EXISTS CASE THEN ELSE VALUES INSERT INTO CREATE TABLE UNIQUE PRIMARY FOREIGN KEY CONSTRAINT INDEX ASC DESC NAME NUMBER ENUMBER STRING AS CROSS

%union
{
   double number;
   char *text;
}


%%

commands: 
        | commands command


command : 
        | select_statement

projection_clause:
                 | ALL  select_list { printf("ALL\n");} 
                 | DISTINCT select_list { printf("DIS\n"); }
                 | select_list
join_options_item: 
                 | join_options
                 | CROSS JOIN table_reference
join_options_list: join_options_list join_options_item
ansi_joined_tables: table_referece: join_options_list

relation_operator: CMP

having_clause : HAVING condition
where_clause : WHERE condition 

table_reference: 
               | NAME 
               | NAME NAME
               | NAME AS NAME 


from_clauses_item:
                 | table_reference
                 | ansi_joined_tables

from_clauses_list: from_clauses_list ',' from_clauses_item

from_clause : FROM from_clauses_list

select_options: 
              | projection_clause from_clause
              | projection_clause from_clause where_clause group_by_clause
              | projection_clause from_clause where_clause group_by_clause having_clause


select_statement: SELECT select_options { printf("Imam select\n");}

%%


void yyerror(char *s) {
    printf("%d: %s at \n", lineno, s);
}

int main(int argc, char* argv[]) {
   yyparse();
 }
