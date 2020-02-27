%{
#include <stdio.h>
#include <string.h>
#include <vector>
#include "token.hxx"
#include "../src/structures/index.h"
#include "../src/structures/table.h"
#include "../src/structures/column.h"
#include "../src/structures/token.h"
using namespace std;
int lineno = 1;
int yylex();
extern char* yytext;
void yyerror(char* msg);
int yyparse();

extern Database* database;
extern vector<SearchType>* searchTypes;
%}


%token SELECT UNION DISTINCT ALL FROM WHERE LIMIT QUOTED_STRING OFFSET ONLY HAVING BY GROUP ORDER JOIN NATURAL LEFT RIGHT INNER FULL OUTER USING CMP BETWEEN NULL_STR IN EXISTS CASE THEN ELSE VALUES INSERT INTO CREATE TABLE UNIQUE PRIMARY FOREIGN KEY CONSTRAINT INDEX ASC DESC NAME NUMBER ENUMBER AS CROSS DATA_TYPE ALTER ADD END WHEN ANY SOME AGG_FUNCTION CHECK UPDATE DELETE SET DEFAULT ON CASCADE REFERENCES IS LIKE
%token SINGLE_QUOTED_STRING

%left OR AND NOT
%left '+' '-'
%left '*' '/'

%error-verbose

%union
{
   double fvalue;
   char *text;  
   Column* column;
   Index* index;
   Table* table;

   int number;

    vector<char*>* names;



}

%type <column> column_definition

%type <table> table_definition
%type <table> create_table_statement
%type <table> list_of_column_definition

%type <index> create_index_statement
%type <index> list_col_index 

%type <text> NAME
%type <text> CMP
%type <number> NUMBER
%type <text> data_types

%type <number> single_column_constraint
%type <number> single_column_item
%type <number> single_column_item_b

%type <table> list_of_column_con_def
%type <index> multiple_column_constraint
%type <index> multiple_column_const_b

%type <names> list_names_sep_comma

%%


commands:
         command ';' commands  
        | command ';'
       
      
        ;

command : 
        select_statement  
        { 
            searchTypes->push_back(SELECT_TYPE);
            printf("SELECT\n");
        }
        | insert_statement
        {
            searchTypes->push_back(INSERT_TYPE);
        }
        | create_table_statement
        {
            database->addTable($1);
            searchTypes->push_back(CREATE_TYPE);
        }
        | create_index_statement 
        {
            Table *t = database->getTable($1->getTable());
            if(t == NULL) {
                yyerror("Ne postoji tablica.");
            }
            t->addIndex($1);
            searchTypes->push_back(CREATE_TYPE);
        }
        | alter_table_statement
        {
            searchTypes->push_back(ALTER_TYPE);
        }
        ;

insert_statement:
                INSERT INTO NAME '(' list_names_sep_comma  ')' values_clause
                {
                    Table *t = database->getTable($3);
                    if(t == NULL) {
                        yyerror("Ne postoji tablica.");
                    }
                    t->addRow();
                }
                | INSERT INTO NAME values_clause 
                {
                    Table *t = database->getTable($3);
                    if(t == NULL) {
                        yyerror("Ne postoji tablica.");
                    }
                    t->addRow();
                }
                ;

alter_table_statement: ALTER TABLE ONLY NAME ADD multiple_column_constraint
                    {
                        Table *table = database->getTable($4);
                        if(table == NULL) {
                            yyerror("Ne postoji tablica.");
                        }
                        if($6 != NULL) {
                            table->addIndex($6);
                        }
                    }
                    |  ALTER TABLE NAME ADD multiple_column_constraint
                     {
                        Table *table = database->getTable($3);
                        if(table == NULL) {
                            yyerror("Ne postoji tablica.");
                        }
                        if($5 != NULL) {
                            table->addIndex($5);
                        }
                     }

list_col_index: 
	      list_col_index ',' NAME ASC 
	        {
                $1->addColumn($3, ASC_ORDER);
                $$=$1;
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
                $$->addColumn($1);
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

create_table_statement: CREATE TABLE NAME table_definition {
    $4->setTableName($3);
    $$=$4;
}

list_of_column_definition: 
			 list_of_column_definition ',' column_definition
       {
          $1->addColumn($3);
          $$ = $1;
       }
			 | column_definition 
       {
          $$ = new Table();
          $$->addColumn($1);
       }
             ;
list_of_column_con_def: 
		      list_of_column_con_def ',' multiple_column_constraint 
              {
                  if($3 != NULL) {
                      $1->addIndex($3);
                  }
                  $$ = $1;
              }
		      | list_of_column_con_def ',' column_definition 
              {
                  $1->addColumn($3);
                  $$=$1;
              }
		      | multiple_column_constraint
              {
                  $$ = new Table();
                  if($1 != NULL) {
                      $$->addIndex($1);
                  }
              }
		      | column_definition 
              {
                  $$ = new Table();
                  $$->addColumn($1);
              }
              ;


table_definition:
		 '(' list_of_column_definition ')'  
         {
             $$ = $2;
         }
		| '(' list_of_column_definition ',' list_of_column_con_def ')'
        {
            $2->mergeTable($4);
            $$=$2;
        }
        ;

data_types: 
             NAME '(' NUMBER ')' {$$=$1;}
          |  NAME '(' NUMBER ',' NUMBER ')' {$$=$1;}
          |  NAME {$$=$1;}
          

column_definition:
                  NAME data_types single_column_constraint 
                  {
                      $$ = new Column($1, $2);
                      if($3 == UNIQUE || $3 == PRIMARY) {
                          
                          $$->setPrimaryOrUnique(true);
                      }
                  }
                 | NAME data_types 
                  {
                      $$ = new Column($1, $2);
                  }
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
                 |  REFERENCES NAME 
                 | REFERENCES NAME '(' list_names_sep_comma ')'
                 
                 ;

multiple_column_const_b:
                        UNIQUE '(' list_names_sep_comma ')'
                        {
                            $$ = new Index();
                            $$->setUnique(true);
                            for(int i = 0; i < $3->size(); ++i) {
                                $$->addColumn((*$3)[i]);
                            }
                        }
                       | PRIMARY KEY '(' list_names_sep_comma ')'
                       {
                            $$ = new Index();
                            for(int i = 0; i < $4->size(); ++i) {
                                $$->addColumn((*$4)[i]);
                            }
                       }
                       | check_clause { $$ = 0;}
                       | FOREIGN KEY '(' list_names_sep_comma  ')' references_clause { $$ = 0;}


multiple_column_constraint:
                           CONSTRAINT NAME multiple_column_const_b { $$ = $3; }
                          | multiple_column_const_b { $$ = $1; }
                          ;

single_column_item_b:
                     NOT NULL_STR {$$ = 0;}
                    | NULL_STR {$$ = 0;}
                    | UNIQUE    { $$ = UNIQUE;}
                    | PRIMARY KEY { $$ = PRIMARY;}
                    | references_clause {$$ = 0;}
                    | check_clause {$$ = 0;}
                    | DEFAULT expression {$$ = 0;}
                    ;

single_column_item:
                   CONSTRAINT NAME single_column_item_b {$$ = $3;}
                  | single_column_item_b {$$ = $1;}
                  ;

single_column_constraint: 
                         single_column_constraint single_column_item
                        | single_column_item
                        ;

subquery: select_statement 
        {
            depth++;
        }

select_list:
            expression AS NAME ',' select_list
           | expression NAME ',' select_list
           | expression NAME
           | expression AS NAME
           | expression ',' select_list
           | expression 

           | '(' subquery ')' ',' select_list
           | '(' subquery ')' AS NAME ',' select_list
           | '(' subquery ')' NAME ',' select_list
           | '(' subquery ')'
           | '(' subquery ')' AS NAME 
           | '(' subquery ')' NAME 
           
           | NAME '.' '*' ',' select_list
           | NAME '.' '*' 
           ;

projection_clause:
                  '*'
                 | select_types  select_list                  
                 | select_list 
                 {
                    
                 }
                 ;

select_types: ALL
            | DISTINCT

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
	     QUOTED_STRING
         | SINGLE_QUOTED_STRING
         ;

condition:
           NOT comparison_condition AND condition
         | NOT comparison_condition OR condition
         | NOT comparison_condition
         
         | NOT condition_with_subquery OR condition
         | NOT condition_with_subquery AND condition
         
         | NOT condition_with_subquery
         | comparison_condition AND condition
         | comparison_condition OR condition
         | comparison_condition { }

         | condition_with_subquery OR condition
         | condition_with_subquery AND condition
         | condition_with_subquery
         | '(' condition ')'  {}
         | condition AND condition
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
		     expression relation_operator expression {printf("comp cond\n");}
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
	  | function_expression {printf("Function\n");}
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
            NAME'.'NAME //{ $$=$1;} //table column
           | NAME  {  variables.push_back(variable($1, depth)); printf("COL %s\n", $1);} //column name
           ;

relation_operator: CMP {printf("CMP %s\n", $1);}

having_clause : HAVING condition

order_type: 
          ASC 
        | DESC

order_item: 
           expression order_type
          | expression 
          | NAME order_type
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
                 {  
                    int len = (strlen($3) + 1) * sizeof(char);
                    char* name = (char*) malloc(len);
                    memset(name, '\0', len);
                    strcpy(name, $3);
                    $1->push_back(name);

                    $$ = $1;
                 }
                | NAME 
                {
                    $$ = new vector<char*>();
                    int len =  (strlen($1) + 1) * sizeof(char);
                    char* name = (char*) malloc(len);
                    memset(name, '\0', len);
                    strcpy(name, $1);
                    $$->push_back(name);
                }
                ;

list_names_num_sep_comma: 
                list_names_num_sep_comma ',' NUMBER
                | list_names_num_sep_comma ',' ENUMBER
                | list_names_num_sep_comma ',' quoted_string
                | quoted_string
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
                NAME { tables.push_back(table_name($1, $1, depth));}
               | NAME NAME { tables.push_back(table_name($2, $1, depth));}
               | NAME AS NAME { tables.push_back(table_name($3, $1, depth));}
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
            {
               
            }

select_options: 
                projection_clause from_clause where_clause group_by_clause having_clause
              | projection_clause from_clause where_clause having_clause
              | projection_clause from_clause where_clause group_by_clause
              | projection_clause from_clause group_by_clause
              | projection_clause from_clause group_by_clause having_clause
              | projection_clause from_clause where_clause 
              | projection_clause from_clause
              | projection_clause
              
              
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
constant: 
           NUMBER 
        | ENUMBER
        | quoted_string
        ;
aggregate_expression: SINGLE_AGG_FUNCTION expression
                    ;

list_function_exp: 
                  column_name ',' list_function_exp
                | column_name
                | function_expression ',' list_function_exp
                | function_expression
 
function_expression: 
                     NAME '(' list_function_exp ')' 
                     {

                     }

                   ;
relational_operator: ""
                   ;

%%

extern FILE *yyin;
Database* database;
vector<SearchType>* searchTypes;

int depth = 0;

TYPE type;
vector<variable> variables;
vector<table_name> tables;

void parse(FILE* fileInput, Database* _database, vector<SearchType>* _searchTypes)
{
   yyin= fileInput;
   database = _database;
   searchTypes = _searchTypes;
   printf("Unutra\n");
   yyparse();
   for(int i=0;i<tables.size();++i) {
       printf("Table: %s\n", tables[i].name);
   }
}

void yyerror(char *s) {
    printf("%d: %s at \"%s\"\n", lineno, s, yytext);
}

/*extern "C"
{
        int yyparse(void);
        int yylex(void); 
}*/


/*int main(int argc, char* argv[]) {
   yyparse();
 }*/
