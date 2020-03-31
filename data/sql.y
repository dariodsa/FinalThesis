%{
#include <stdio.h>
#include <string.h>
#include <vector>

#include "token.cpp"
#include "../src/structures/index.h"
#include "../src/structures/table.h"
#include "../src/structures/column.h"
extern "C++" {
    #include "../src/structures/result.h"
}

using namespace std;

int lineno = 1;
int yylex();
extern char* yytext;
void yyerror(const char* msg);
int yyparse();

extern Database* database;

extern vector<SearchType>* searchTypes;
%}


%token SELECT UNION DISTINCT ALL FROM WHERE LIMIT QUOTED_STRING OFFSET ONLY HAVING BY GROUP ORDER JOIN NATURAL LEFT RIGHT INNER FULL OUTER USING CMP BETWEEN NULL_STR IN EXISTS CASE THEN ELSE VALUES INSERT INTO CREATE TABLE UNIQUE PRIMARY FOREIGN KEY CONSTRAINT INDEX ASC DESC NAME NUMBER ENUMBER AS CROSS DATA_TYPE ALTER ADD END WHEN ANY SOME AGG_FUNCTION CHECK UPDATE DELETE SET DEFAULT ON CASCADE REFERENCES IS LIKE
%token SINGLE_QUOTED_STRING SINGLE_AGG_FUNCTION

%left OR
%left '+' '-' AND
%left '*' '/' NOT


%error-verbose

%union
{
    double fvalue;
    char *text;  
    Column* column;
    Index* index;
    Table* table;



    int number;

    vector<table_name*>* tables;
    vector<char*>* names;

    table_name* table_name;

    variable* variable;
    expression_info* expression_info;

    node *node;
    Select* select;
}



%type <column> column_definition

%type <table> table_definition
%type <table> create_table_statement
%type <table> list_of_column_definition

%type <index> create_index_statement
%type <index> list_col_index 

%type <text> NAME
%type <text> CMP
%type <text> relation_operator
%type <number> NUMBER
%type <text> data_types

%type <number> single_column_constraint
%type <number> single_column_item
%type <number> single_column_item_b

%type <table> list_of_column_con_def
%type <index> multiple_column_constraint
%type <index> multiple_column_const_b

%type <names> list_names_sep_comma

%type <table_name> table_reference

%type <table_name> join_options_item 
%type <table_name> join_options
%type <table_name> join_options_part_two

%type <tables> from_clauses_item
%type <tables> join_options_list
%type <tables> ansi_joined_tables
%type <tables> from_clauses_list

%type <variable> column_name

%type <expression_info> expression_part_two
%type <expression_info> expression
%type <expression_info> comparison_condition
%type <expression_info> in_condition

%type <expression_info> condition_with_subquery

%type <expression_info> aggregate_expression

%type <expression_info> function_expression
%type <expression_info> list_function_exp

%type <node> condition
%type <node> where_clause

%type <select> select_options
%type <select> select_options_more
%type <select> select_statement
%type <select> subquery
%type <select> projection_clause
%type <select> select_list

%type <number> limit_offset_clause
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
            select_result = $1;
            
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
                return 1;
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
                        return 1;
                    }
                    t->addRow();
                }
                | INSERT INTO NAME values_clause 
                {
                    Table *t = database->getTable($3);
                    if(t == NULL) {
                        yyerror("Ne postoji tablica.");
                        return 1;
                    }
                    t->addRow();
                }
                ;

alter_table_statement: ALTER TABLE ONLY NAME ADD multiple_column_constraint
                    {
                        Table *table = database->getTable($4);
                        if(table == NULL) {
                            yyerror("Ne postoji tablica.");
                            return 1;
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
                            return 1;
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
            $$ = $1;
        }

select_list:
            expression AS NAME ',' select_list
            {
                $$ = $5;
            }
           | expression NAME ',' select_list
           {
               $$ = $4;
           }
           | expression NAME
           {
               $$ = new Select();
           }
           | expression AS NAME
           {
               $$ = new Select();
           }
           | expression ',' select_list
           {
               $$ = $3;
           }
           | expression 
           {
               $$ = new Select();
           } 
           | '(' subquery ')' ',' select_list
           {
               $$ = $2;
               $$->addSibling($5);
           }
           | '(' subquery ')' AS NAME ',' select_list
           {
               $$ = $2;
               $$->addSibling($7);
           }
           | '(' subquery ')' NAME ',' select_list
           {
               $$ = $2;
               $$->addSibling($6);
           }
           | '(' subquery ')'
           {
               $$ = $2;
           }
           | '(' subquery ')' AS NAME 
           {
               $$ = $2;
           }
           | '(' subquery ')' NAME 
           {
               $$ = $2;
           }
           | NAME '.' '*' ',' select_list
           {
               $$ = $5;
           }
           | NAME '.' '*' 
           {
               $$ = new Select();
           }
           ;

projection_clause:
                  '*'
                  {
                      $$ = new Select();
                  }
                 | select_types  select_list                  
                 {
                    $$ = $2;
                 }
                 | select_list 
                 {
                    $$ = $1;
                 }
                 ;

select_types: ALL
            | DISTINCT

join_options: 
             join_options_part_one join_options_part_two { $$ = $2;}
            | NATURAL join_options_part_one join_options_part_two { $$ = $3;}
            | NATURAL join_options_part_one JOIN table_reference ON { $$ = $4;}
            | join_options_part_two { $$ = $1; }
            ;

join_options_part_two: 
                    JOIN table_reference ON condition 
                    {
                        $$ = $2;
                        
                    }
                    | JOIN table_reference ON USING '(' list_names_sep_comma  ')' 
                    {
                        $$ = $2;
                    }
                    ;           

join_options_part_one: 
                     INNER 
                    | LEFT OUTER
                    | RIGHT OUTER 
                    | FULL OUTER 
                    ;

join_options_item: 
                  join_options { $$ = $1; }
                 | CROSS JOIN table_reference 
                 { 
                     $$ = $3;
                 }
                 ;

join_options_list: join_options_list join_options_item 
                {             
                    $1->push_back($2);
                    $$ = $1;
                }
                 | join_options_item 
                 { 
                     $$ = new vector<table_name*>();
                     $$->push_back($1);
                 }
ansi_joined_tables:  table_reference join_options_list
                {
                    $2->push_back($1);
                    $$ = $2;
                }

quoted_string: 
	     QUOTED_STRING
         | SINGLE_QUOTED_STRING
         ;

condition:
           NOT comparison_condition AND condition
         {
             
            
            node* n = new node();
            strcpy(n->name, NOT_STR);
            n->left = new node();
            n->left->e1 = $2; 


            $$ = new node();
            strcpy($$->name, AND_STR);
            $$->right = $4;
            $$->left = n;

         }
         | NOT comparison_condition OR condition
         {
            node* n = new node();
            strcpy(n->name, NOT_STR);
            n->left = new node();
            n->left->e1 = $2; 


            $$ = new node();
            strcpy($$->name, OR_STR);
            $$->right = $4;
            $$->left = n;
         }
         | NOT comparison_condition
         {
            $$ = new node();
            strcpy($$->name, NOT_STR);
            $$->left = new node();
            $$->left->terminal = true;
            $$->left->e1 = $2;
         }         
         | NOT condition {
            node* n1 = $2;

            $$ = new node();
            strcpy($$->name, NOT_STR);
            $$->left = n1;
         }
         | NOT condition_with_subquery OR condition
         {
            node* n1 = new node();
            n1->terminal = true;
            n1->e1 = $2;

            node* n2 = new node();
            strcpy(n2->name, NOT_STR);
            n2->left = n1;

            $$ = new node();
            strcpy($$->name, OR_STR);
            $$->left = n2;
            $$->right = $4;
         }
         | NOT condition_with_subquery AND condition
         {
            node* n1 = new node();
            n1->terminal = true;
            n1->e1 = $2;

            node* n2 = new node();
            strcpy(n2->name, NOT_STR);
            n2->left = n1;

            $$ = new node();
            strcpy($$->name, AND_STR);
            $$->left = n2;
            $$->right = $4;
         }
         | NOT condition_with_subquery
         {
            node* n1 = new node();
            n1->terminal = true;
            n1->e1 = $2;

            $$ = new node();
            strcpy($$->name, NOT_STR);
            $$->left = n1;
         }
         | comparison_condition AND condition
         {
            node* n1 = new node();
            n1->terminal = true;
            n1->e1 = $1;

            $$ = new node();
            strcpy($$->name, AND_STR);
            $$->left = n1;
            $$->right = $3;
         }
         | comparison_condition OR condition
         {
            node* n1 = new node();
            n1->terminal = true;
            n1->e1 = $1;

            $$ = new node();
            strcpy($$->name, OR_STR);
            $$->left = n1;
            $$->right = $3;
         }
         | comparison_condition 
         {
            $$ = new node();
            $$->terminal = true;
            $$->e1 = $1;
         }
         | condition_with_subquery OR condition
         {
            node* n1 = new node();
            n1->terminal = true;
            n1->e1 = $1;

            $$ = new node();
            strcpy($$->name, OR_STR);
            $$->left = n1;
            $$->right = $3;
         }
         | condition_with_subquery AND condition
         {
            node* n1 = new node();
            n1->terminal = true;
            n1->e1 = $1;

            $$ = new node();
            strcpy($$->name, AND_STR);
            $$->left = n1;
            $$->right = $3;
         }
         | condition_with_subquery
         {
            $$ = new node();
            $$->terminal = true;
            $$->e1 = $1;
         }
         | '(' condition ')'  { $$ = $2; }
         | condition AND condition
         {
            $$ = new node();
            strcpy($$->name, AND_STR);
            $$->left = $1;
            $$->right = $3;
         }
         | condition OR condition
         {
            $$ = new node();
            strcpy($$->name, OR_STR);
            $$->left = $1;
            $$->right = $3;
         }
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
             {  
                $$ = $1;
                if(strcmp("=", $2) == 0) $$->equal = 1;
                else if(strcmp("<>", $2) == 0) $$->equal = 2;
                else $$->equal = 0;
                for(variable v : *$3->variables) {
                    $$->variables->push_back(v);
                }
                printf("%s %d\n", $2, $$->equal);
             }
		    | expression NOT BETWEEN expression AND expression 
            {
                $$ = $1;
                for(variable v : *$4->variables) {
                     $$->variables->push_back(v);
                }
                for(variable v : *$6->variables) {
                     $$->variables->push_back(v);
                }
                $$->equal = 0;

            }
		    | expression BETWEEN expression AND expression
            {
                $$ = $1;
                for(variable v : *$3->variables) {
                     $$->variables->push_back(v);
                }
                for(variable v : *$5->variables) {
                     $$->variables->push_back(v);
                }
                $$->equal = 0;
            }
		    | in_condition
		    {
                $$ = $1;
                $$->equal = 0;
            }
            | column_name IS NULL_STR
            {
                $$ = new expression_info();
                $$->variables->push_back(*$1);
                $$->equal = 1;
            }
		    | column_name IS NOT NULL_STR
            {
                $$ = new expression_info();
                $$->variables->push_back(*$1);
                $$->equal = 2;
            }
		    | quoted_string NOT LIKE quoted_string
            {
                $$ = new expression_info();
            }
		    | quoted_string LIKE quoted_string 
            {
                $$ = new expression_info();
            }
		    | column_name NOT LIKE quoted_string
            {
                $$ = new expression_info();
                $$->variables->push_back(*$1);
                $$->equal = 0;
            }
		    | column_name LIKE quoted_string 
            {
                $$ = new expression_info();
                $$->variables->push_back(*$1);
                $$->equal = 0;
            }
		    | quoted_string NOT LIKE column_name
            {
                $$ = new expression_info();
                $$->variables->push_back(*$4);
                $$->equal = 0;
            }
		    | quoted_string LIKE column_name
            {
                $$ = new expression_info();
                $$->variables->push_back(*$3);
                $$->equal = 0;
            }
		    | column_name NOT LIKE column_name
            {
                $$ = new expression_info();
                $$->variables->push_back(*$1);
                $$->variables->push_back(*$4);
                $$->equal = 0;
            }
		    | column_name LIKE column_name
            {
                $$ = new expression_info();
                $$->variables->push_back(*$1);
                $$->variables->push_back(*$3);
                $$->equal = 0;
            }
            ;
expression: 
	   expression_part_one expression_part_two { $$ = $2; }
	  | expression_part_two { $$ = $1; }
      ;
expression_part_one:
		    '-'
		   | '+'
           ;

expression_part_two: 
	   column_name binary_operator expression
       {
           $3->variables->push_back(*$1);
           $$ = $3;
       }
	  | column_name 
      {
          $$ = new expression_info();
          $$->variables->push_back(*$1); 
      }
	  | conditional_expression binary_operator expression
      {
        $$ = new expression_info();
      }
	  | conditional_expression 
      {
        $$ = new expression_info(); 
      }
	  | constant binary_operator expression { $$ = $3; }
	  | constant { $$ = new expression_info(); }
	  | aggregate_expression binary_operator expression
      {
        $$ = $3;
        for(variable v : *$1->variables) {
            $$->variables->push_back(v);
        }
      }
	  | aggregate_expression { $$ = $1; }
	  | function_expression binary_operator expression
      {
        $$ = $3;
        for(variable v : *$1->variables) {
            $$->variables->push_back(v);
        }
      }
	  | function_expression {printf("Function\n");}
	  | NULL_STR binary_operator expression { $$ = $3; }
	  | NULL_STR { $$ = new expression_info(); }
	  | '(' expression ')' binary_operator expression 
      {

      }
	  | '(' expression ')' { $$ = $2; }
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
                       {
                           $$ = $1;
                           //TODO add result of subquery to the some tree structure
                       }
                       | expression IN '(' subquery ')'
                       {
                           $$ = $1;
                           //TODO add result of subquery to the some tree structure
                       }
                       | expression relation_operator '(' subquery ')'
                       {
                           $$ = $1;
                           //TODO add result of subquery to the some tree structure
                       }
                       | expression relation_operator all_any_some '(' subquery ')'
                       {
                           $$ = $1;
                           //TODO add result of subquery to the some tree structure
                       }
                       | EXISTS '(' subquery ')'
                       {
                           $$ = new expression_info();
                           //TODO add result of subquery to the some tree structure

                       }
                       ;

limit_offset_clause: 
                    LIMIT expression
                   | LIMIT expression OFFSET  expression
                   ;

column_name: 
            NAME'.'NAME 
            {
                bool s = (*sp)->SELECT_LIST;
                variable* V = new variable($3, $1, depth);
                if(s) {
                    (*sp)->select_variable->push_back(*V);
                    $$ = V;
                } else {
                    vector<table_name*>* tables = (*sp)->tables;
                    for(table_name* t1 : *tables) {
                        if(strcmp($1, t1->name) == 0 || strcmp($1, t1->real_name) == 0) {
                            V->setTable(t1->real_name);
                        }
                    }
                    $$ = V;
                }
            }
           | NAME  
             {  
               bool s = (*sp)->SELECT_LIST;
               variable* V = new variable($1, depth);
               if(s) {
                   (*sp)->select_variable->push_back(*V);
                   $$ = V;
               } else {
                    vector<table_name*>* tables = (*sp)->tables;
                    
                    char* table_name = database->getTableNameByVar($1, tables);
                    if(table_name == NULL) {
                        yyerror("Ne postoji stupac pod nazivom.");
                        return 1;
                    }
                    V->setTable(table_name);
                    $$ = V;
               }
             }
           ;

relation_operator: CMP {}

having_clause: HAVING condition
        {
            
        }

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

columns_sep_comma: column_name ',' columns_sep_comma
                {
                    (*sp)->select_variable->push_back(*$1);
                }
                | column_name 
                {
                    (*sp)->select_variable->push_back(*$1);
                }

group_by_clause: GROUP BY columns_sep_comma 
            {

            }

where_clause: WHERE condition 
            {
                $$ = $2;
            }

table_reference: 
                NAME { $$ = new table_name($1, $1, depth); }
               | NAME NAME { $$ = new table_name($2, $1, depth); }
               | NAME AS NAME { $$ = new table_name($3, $1, depth); }
               ;

from_clauses_item:
                 table_reference 
                 { 
                     $$ = new vector<table_name*>();
                     $$->push_back($1); 
                 }
                 | ansi_joined_tables { $$ = $1; }
                 ;

from_clauses_list: 
                  from_clauses_list ',' from_clauses_item 
                  { 
                      for(int i = 0, len = $3->size(); i < len; ++i) {
                        $1->push_back((*$3)[i]);    
                      }
                      $$ = $1;
                  }
                | from_clauses_item 
                { 
                    $$ = $1;
                }
                 ;

from_clause: FROM from_clauses_list 
            {
                //tables.push(vector<table_name>());
                for(int i = 0; i < $2->size(); ++i) {
                    printf("Table: %s %s\n", (*$2)[i]->name, (*$2)[i]->real_name);
                }
                (*sp)->tables = $2;
                (*sp)->SELECT_LIST = false;
                for(variable v : *((*sp)->select_variable)) {
                   printf("Variable: %s %s\n", v.name, v.table);
                }
            }

select_options: 
                projection_clause from_clause where_clause group_by_clause having_clause
                {
                    Select* s = new Select(database, $3, (*sp)->tables, (*sp)->select_variable);
                    s->addKid($1);
                    s->addGroup();
                    $$ = s;
                }
                | projection_clause from_clause where_clause group_by_clause
                {
                    Select* s = new Select(database, $3, (*sp)->tables, (*sp)->select_variable);
                    s->addKid($1);
                    s->addGroup();
                    $$ = s;
                }
                | projection_clause from_clause group_by_clause
                {
                    node* root = 0;
                    Select* s = new Select(database, root, (*sp)->tables, (*sp)->select_variable);
                    s->addKid($1);
                    s->addGroup();
                    $$ = s;
                }
                | projection_clause from_clause group_by_clause having_clause
                {
                    node* root = 0;
                    Select* s = new Select(database, root, (*sp)->tables, (*sp)->select_variable);
                    s->addKid($1);
                    s->addGroup();
                    $$ = s;
                }
                | projection_clause from_clause where_clause 
                {
                    Select* s = new Select(database, $3, (*sp)->tables, (*sp)->select_variable);
                    s->addKid($1);
                    $$ = s;
                }
                | projection_clause from_clause
                {
                    node* root = 0;
                    Select* s = new Select(database, root, (*sp)->tables, (*sp)->select_variable);
                    s->addKid($1);
                    $$ = s;
                }
              ;

select_options_more:
                    UNION ALL SELECT select_options { $$ = $4; }
                   | UNION SELECT select_options { $$ = $3; }
                   ;

select_statement:  //Select* s = new Select(database, $2, (*sp)->tables, (*sp)->select_variable);
                 select_word select_options
                 {
                     $$ = $2;
                     --depth;
                     pop(sp);
                 }
                | select_word select_options select_options_more order_by_clause limit_offset_clause
                {
                    $$ = $2;
                    $$->addSort();
                    $$->addLimit($5);
                    $$->addSibling($3);
                    --depth;
                    pop(sp);
                }
                | select_word select_options select_options_more order_by_clause
                {
                    $$ = $2;
                    $$->addSort();
                    $$->addSibling($3);
                    --depth;
                    pop(sp);
                }
                | select_word select_options order_by_clause limit_offset_clause
                {
                    $2->addSort();
                    $2->addLimit($4);
                    $$ = $2;
                    --depth;
                    pop(sp);
                }
                | select_word select_options limit_offset_clause
                {
                    $2->addLimit($3);
                    $$ = $2;
                    --depth;
                    pop(sp);
                }
                | select_word select_options order_by_clause
                {
                    $2->addSort();
                    $$ = $2;
                    --depth;
                    pop(sp);
                }
                ;

select_word: SELECT {
    auto s1 = new select_state();
    printf("new select state\n");
    push(sp, s1);
    printf("select new %d\n", *(*sp));
    depth++;
}

binary_operator: ""
               ;
constant: 
           NUMBER 
        | ENUMBER
        | quoted_string
        ;
aggregate_expression: SINGLE_AGG_FUNCTION expression { $$ = $2; }
                    ;

list_function_exp: 
                  column_name ',' list_function_exp
                {
                    $$ = $3;
                    $$->variables->push_back(*$1);
                }
                | column_name 
                {
                    $$ = new expression_info();
                    $$->variables->push_back(*$1);
                }
                | function_expression ',' list_function_exp
                {
                    $$ = $1;
                    for(variable v : *$3->variables) {
                        $$->variables->push_back(v);
                    }
                }
                | function_expression 
                {
                    $$ = $1;
                }
 
function_expression: 
                     NAME '(' list_function_exp ')' 
                     {
                        $$ = $3;
                        $$->locked = true;
                     }

                   ;

%%

extern FILE *yyin;
Database* database;
vector<SearchType>* searchTypes;


int depth = 0;

bool SELECT_LIST = false;

select_state* stack[1000];
select_state** sp;
#define push(sp, n) (*(++(sp)) = (n))
#define pop(sp) (*--(sp))

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

Select* select_result = 0;

Select* parse(const char* query, Database* _database, vector<SearchType>* _searchTypes)
{
    sp = stack;
    database = _database;
    searchTypes = _searchTypes;
    printf("Unutra\n");

    YY_BUFFER_STATE buffer = yy_scan_string(query);

    int res = yyparse();
    yy_delete_buffer(buffer);
    
    return select_result;
}

void yyerror(const char *s) {
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

