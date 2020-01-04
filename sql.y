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
        | insert_statement
        | create_table_statement
        | create_index_statement
        | alter_table_statement

create_table_statement: CREATE TABLE NAME table_definition

list_of_column_definition: 
			 | list_of_column_definition ',' column_definition
			 | column_definition
list_of_column_con_def: 
		      | list_of_column_con_def ',' multiple_column_constraint 
		      | list_of_column_con_def ',' column_defintion 
		      | multiple_column_constraint
		      | column_definition

alter_table_statement: ALTER TABLE NAME ADD CONSTRAINT multiple_column_constraint

list_col_index: 
	      | list_col_index ',' NAME ASC
	      | list_col_index ',' NAME DESC
	      | list_col_index ',' NAME

create_index_statement: 
		      | CREATE INDEX NAME ON NAME '(' list_col_index ')'
		      | CREATE UNIQUE INDEX NAME ON NAME '(' list_col_index ')'

table_definition:
		| '(' list_of_column_definition ')'  
		| '(' list_of_column_definition ',' list_of_column_con_def ')'


column_definition:
                 | NAME DATA_TYPE
                 | NAME DATA_TYPE single_column_constraint

single_column_constraint: 
                        | 

projection_clause:
                 | ALL  select_list { printf("ALL\n");} 
                 | DISTINCT select_list { printf("DIS\n"); }
                 | select_list

join_options: 
            | join_options_part_one join_options_part_two
            | NATURAL join_options_part_one join_options_part_two
            | NATURAL join_options_part_one JOIN table_reference ON
            | join_options_part_two 

join_options_part_two: 
                     | JOIN table_reference ON condition
                     | JOIN table_reference ON USING '(' list_names_sep_comma  ')'
           
join_options_part_one: 
                    | INNER 
                    | LEFT OUTER
                    | RIGHT OUTER 
                    | FULL OUTER 

join_options_item: 
                 | join_options
                 | CROSS JOIN table_reference
join_options_list: join_options_list join_options_item
ansi_joined_tables:  table_reference join_options_list

condition:
         | NOT comparison_condition
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

limit_offset_clause: 
                   | LIMIT expression
                   | LIMIT expression OFFSET  expression

column_name: 
           | NAME NAME //table column
           | NAME //column name

relation_operator: CMP

having_clause : HAVING condition

order_item: 
          | expression ASC
          | expression DESC
          | expression 
          | NAME ASC
          | NAME DESC
          | NAME
order_list: 
          | order_list ',' order_item
          | order_item

order_by_clause:
               | ORDER BY order_list

list_names_sep_comma: 
                    | list_names_sep_comma ',' NAME 
                    | NAME
values_clause:
             | VALUES '(' list_names_sep_comma ')'
             | VALUES '(' NULL_STR ')'

group_by_clause: GROUP BY list_names_sep_comma

where_clause : WHERE condition 

table_reference: 
               | NAME 
               | NAME NAME
               | NAME AS NAME 


from_clauses_item:
                 | table_reference
                 | ansi_joined_tables

from_clauses_list: 
                 | from_clauses_list ',' from_clauses_item
                 | from_clauses_item

from_clause : FROM from_clauses_list

select_options: 
              | projection_clause from_clause
              | projection_clause from_clause where_clause group_by_clause
              | projection_clause from_clause where_clause group_by_clause having_clause

select_options_more:
                   | UNION ALL SELECT select_options
                   | UNION SELECT select_options

select_statement: 
                | SELECT select_options
                | SELECT select_options select_options_more order_by_clause limit_offset_clause
                | SELECT select_options select_options_more order_by_clause
                | SELECT select_options order_by_clause limit_offset_clause
                | SELECT select_options limit_offset_clause
                | SELECT select_options order_by_clause

%%


void yyerror(char *s) {
    printf("%d: %s at \n", lineno, s);
}

int main(int argc, char* argv[]) {
   yyparse();
 }
