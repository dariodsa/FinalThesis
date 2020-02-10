%{
#include <stdio.h>
#include <string.h>
#include <vector>
#include "token.h"
#include "../src/structures/index.h"
#include "../src/structures/table.h"
using namespace std;
int lineno = 1;
int yylex();
extern char* yytext;
void yyerror(char* msg);
int yyparse();

extern Database* database;

%}


%token SELECT UNION DISTINCT ALL FROM WHERE LIMIT OFFSET HAVING BY GROUP ORDER JOIN NATURAL LEFT RIGHT INNER FULL OUTER USING CMP BETWEEN NULL_STR IN EXISTS CASE THEN ELSE VALUES INSERT INTO CREATE TABLE UNIQUE PRIMARY FOREIGN KEY CONSTRAINT INDEX ASC DESC NAME NUMBER ENUMBER STRING AS CROSS DATA_TYPE ALTER ADD END WHEN ANY SOME AGG_FUNCTION CHECK UPDATE DELETE SET DEFAULT ON CASCADE REFERENCES IS LIKE

%left OR AND NOT
%left '+' '-'
%left '*' '/'



%union
{
   double number;
   char *text;  
   Index* index;
   struct token {
      int s1;
      int s2;
   } s;
}

%type <index> create_index_statement
%type <index> list_col_index 
%type <text> NAME


%%


commands: 
        command //{$$ = $1;} //only one command at time
        ;

command : 
        select_statement  { printf("SELECET\n");}
        | insert_statement
        | create_table_statement
        | create_index_statement 
            {
                Table *t = database->getTable($1->getTable());
                t->addIndex($1);
            }
        | alter_table_statement
        ;

insert_statement:
                INSERT INTO NAME '(' list_names_sep_comma  ')' values_clause
                | INSERT INTO NAME '(' list_names_sep_comma ')' select_statement
                | INSERT INTO NAME values_clause 
                | INSERT INTO NAME select_statement
                ;

create_table_statement: CREATE TABLE NAME table_definition

list_of_column_definition: 
			 list_of_column_definition ',' column_definition
			 | column_definition
             ;
list_of_column_con_def: 
		      list_of_column_con_def ',' multiple_column_constraint 
		      | list_of_column_con_def ',' column_definition 
		      | multiple_column_constraint
		      | column_definition
              ;

alter_table_statement: ALTER TABLE NAME ADD CONSTRAINT multiple_column_constraint

list_col_index: 
	      list_col_index ',' NAME ASC 
	        {
                $1->addColumn($3, ASC_ORDER);
                $$=$1;
                printf("String value %s ASC\n", $3);
            }
          | list_col_index ',' NAME DESC
	        {
                $1->addColumn($3, DESC_ORDER);
                $$=$1;
            }
          | list_col_index ',' NAME
            {
                $1->addColumn($3, ASC_ORDER);
                $$=$1;
            }
          | NAME ASC
            {
                $$=new Index();
                $$->addColumn($1, ASC_ORDER); 
            }
          | NAME DESC
            {
                $$=new Index();
                $$->addColumn($1, DESC_ORDER); 
            }
          | NAME 
            {
                $$=new Index();
                $$->addColumn($1, ASC_ORDER);
            }
          ;

create_index_statement: 
		      CREATE INDEX NAME ON NAME '(' list_col_index ')' 
                {
                    $$=$7;
                    $$->setTable($5);
                    $$->setName($3);
                    $$->setUnique(false);
                }
		      | CREATE UNIQUE INDEX NAME ON NAME '(' list_col_index ')' 
                {
                    $$=$8;
                    $$->setTable($6);
                    $$->setName($4);
                    $$->setUnique(true);
                }
              ;

table_definition:
		 '(' list_of_column_definition ')'  
		| '(' list_of_column_definition ',' list_of_column_con_def ')'
        ;

column_definition:
                  NAME STRING //DATA_TYPE
                 | NAME STRING single_column_constraint
                 ;

check_clause: CHECK '(' condition ')'

references_clause_part_a:
                         ON DELETE
                        | ON UPDATE
                        ;

references_clause_part_b:
                         CASCADE
                        | SET NULL_STR
                        | SET DEFAULT
                        ;

references_clause_part_two:
                           references_clause_part_two references_clause_part_a references_clause_part_b
                          | references_clause_part_a references_clause_part_b
                          ;

references_clause:
                  REFERENCES NAME references_clause_part_two
                 | REFERENCES NAME '(' list_names_sep_comma ')' references_clause_part_two
                 ;

multiple_column_const_b:
                        UNIQUE '(' list_names_sep_comma ')'
                       | PRIMARY KEY '(' list_names_sep_comma ')'
                       | check_clause
                       | FOREIGN KEY '(' list_names_sep_comma  ')' references_clause


multiple_column_constraint:
                           CONSTRAINT NAME multiple_column_const_b
                          | multiple_column_const_b
                          ;

single_column_item_b:
                     NOT NULL_STR
                    | UNIQUE    
                    | PRIMARY KEY
                    | references_clause
                    | check_clause
                    | DEFAULT expression
                    ;

single_column_item:
                   CONSTRAINT NAME single_column_item_b
                  | single_column_item_b
                  ;

single_column_constraint: 
                         single_column_constraint single_column_item
                        | single_column_item
                        ;

subquery: select_statement

select_list:
            expression AS NAME ',' select_list
           | expression AS NAME select_list
           | expression NAME ',' select_list
           | expression NAME
           | '(' subquery ')' select_list
           | '(' subquery ')' AS NAME select_list
           | '(' subquery ')' NAME select_list
           | '(' subquery ')'
           | '(' subquery ')' AS NAME 
           | '(' subquery ')' NAME 
           | expression ',' select_list
           | expression 
           | NAME '*' select_list
           | NAME '*' 
           ;

projection_clause:
                  '*'
                 | ALL  select_list { printf("ALL\n");} 
                 | DISTINCT select_list { printf("DIS\n"); }
                 | select_list
                 ;

join_options: 
             join_options_part_one join_options_part_two
            | NATURAL join_options_part_one join_options_part_two
            | NATURAL join_options_part_one JOIN table_reference ON
            | join_options_part_two 
            ;

join_options_part_two: 
                      JOIN table_reference ON condition
                     | JOIN table_reference ON USING '(' list_names_sep_comma  ')'
                     ;           

join_options_part_one: 
                     INNER 
                    | LEFT OUTER
                    | RIGHT OUTER 
                    | FULL OUTER 
                    ;

join_options_item: 
                  join_options
                 | CROSS JOIN table_reference
                 ;

join_options_list: join_options_list join_options_item
ansi_joined_tables:  table_reference join_options_list

quoted_string: 
	      '"' NAME '"'
	     | '"' NUMBER '"'
	     | '"' ENUMBER '"'
	     | '"' STRING '"' 
         ;

condition:
          NOT comparison_condition
         | NOT condition_with_subquery
         | comparison_condition
         | condition_with_subquery
         | condition
         | NOT comparison_condition AND condition
         | NOT condition_with_subquery AND condition
         | comparison_condition AND condition
         | condition_with_subquery AND condition
         | condition AND condition
         | NOT comparison_condition OR condition
         | NOT condition_with_subquery OR condition
         | comparison_condition OR condition
         | condition_with_subquery OR condition
         | condition OR condition
         ;

list_condition_expression: 
                          list_condition_expression WHEN condition THEN expression 
                         | WHEN condition THEN expression
                         ;
list_expression_expression:
                           list_expression_expression WHEN expression THEN expression
                          | WHEN expression THEN expression
                          ;
conditional_expression: 
                       CASE list_condition_expression END
                      | CASE list_condition_expression ELSE expression END
                      | CASE expression list_expression_expression END
                      | CASE expression list_expression_expression ELSE expression END
                      ;
comparison_condition: 
		     expression relation_operator expression 
		    | expression NOT BETWEEN expression AND expression 
		    | expression BETWEEN expression AND expression
		    | in_condition
		    | column_name IS NULL_STR
		    | column_name IS NOT NULL_STR
		    | quoted_string NOT LIKE quoted_string
		    | quoted_string LIKE quoted_string 
		    | column_name NOT LIKE quoted_string
		    | column_name LIKE quoted_string 
		    | quoted_string NOT LIKE column_name
		    | quoted_string LIKE column_name
		    | column_name NOT LIKE column_name
		    | column_name LIKE column_name
            ;
expression: 
	   expression_part_one expression_part_two
	  | expression_part_two
      ;
expression_part_one:
		    '-'
		   | '+'
           ;

expression_part_two: 
	   column_name binary_operator expression
	  | column_name 
	  | conditional_expression binary_operator expression
	  | conditional_expression
	  | constant binary_operator expression
	  | constant
	  | aggregate_expression binary_operator expression
	  | aggregate_expression 
	  | function_expression binary_operator expression
	  | function_expression
	  | NULL_STR binary_operator expression
	  | NULL_STR
	  | '(' expression ')' binary_operator expression 
	  | '(' expression ')' 
      ;

in_condition: 
             expression NOT IN '(' list_names_num_sep_comma ')'
            | expression IN '(' list_names_num_sep_comma ')'
            ;

all_any_some: 
             ALL
            | ANY
            | SOME
            ;

condition_with_subquery: 
                       expression NOT IN '(' subquery ')'
                       | expression IN '(' subquery ')'
                       | EXISTS '(' subquery ')'
                       | expression relational_operator '(' subquery ')'
                       | expression relational_operator all_any_some '(' subquery ')'
                       ;

limit_offset_clause: 
                    LIMIT expression
                   | LIMIT expression OFFSET  expression
                   ;

column_name: 
            NAME NAME //{ $$=$1;} //table column
           | NAME  //{ $$=$1;} //column name
           ;

relation_operator: CMP

having_clause : HAVING condition

order_item: 
           expression ASC
          | expression DESC
          | expression 
          | NAME ASC
          | NAME DESC
          | NAME
          ;
order_list: 
           order_list ',' order_item
          | order_item
          ;

order_by_clause:
                ORDER BY order_list
               ;

list_names_sep_comma: 
                 list_names_sep_comma ',' NAME 
                | NAME
                ;

list_names_num_sep_comma: 
                 list_names_num_sep_comma ',' NAME 
                | list_names_num_sep_comma ',' NUMBER
                | list_names_num_sep_comma ',' ENUMBER
                | NAME
                | NUMBER
                | ENUMBER
                ;


values_clause:
              VALUES '(' list_names_num_sep_comma ')'
             | VALUES '(' NULL_STR ')'
             ;

group_by_clause: GROUP BY list_names_sep_comma

where_clause : WHERE condition 

table_reference: 
                NAME 
               | NAME NAME
               | NAME AS NAME 
               ;

from_clauses_item:
                 table_reference
                 | ansi_joined_tables
                 ;

from_clauses_list: 
                  from_clauses_list ',' from_clauses_item
                 | from_clauses_item
                 ;

from_clause : FROM from_clauses_list

select_options: 
               projection_clause from_clause
              | projection_clause from_clause where_clause group_by_clause
              | projection_clause from_clause where_clause group_by_clause having_clause
              ;

select_options_more:
                    UNION ALL SELECT select_options
                   | UNION SELECT select_options
                   ;

select_statement: 
                 SELECT select_options
                | SELECT select_options select_options_more order_by_clause limit_offset_clause
                | SELECT select_options select_options_more order_by_clause
                | SELECT select_options order_by_clause limit_offset_clause
                | SELECT select_options limit_offset_clause
                | SELECT select_options order_by_clause
                ;

binary_operator: ""
               ;
constant: ""
        ;
aggregate_expression: ""
                    ;
function_expression: ""
                   ;
relational_operator: ""
                   ;

%%

extern FILE *yyin;
Database* database;

void parse(FILE* fileInput, Database* _database)
{
   yyin= fileInput;
   database = _database;
   printf("Unutra\n");
   printf("%d\n", yyin);
   yyparse();
   
}

void yyerror(char *s) {
    printf("%d: %s at %s\n", lineno, s, yytext);
}

/*extern "C"
{
        int yyparse(void);
        int yylex(void); 
}*/


/*int main(int argc, char* argv[]) {
   yyparse();
 }*/
