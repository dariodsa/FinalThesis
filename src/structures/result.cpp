#include "result.h"
#include <stdio.h>
#include <queue>
#include <string>
using namespace std;

char AND_STR[] = "AND";
char NOT_STR[] = "NOT";
char OR_STR[]  = "OR";

Result::Result() {}

Result::Result(Table* table, std::vector<Column*>* columns) {
    this->table = table;
    for(Column* c : *columns) {
        this->columns.push_back(c);
    }
}

void Result::print() {
    printf("u %s\n",  table->getTableName());
    printf(" nad retcima: ");
    for(Column* column : this->columns) {
        printf("%s, ", column->getName());
    }
    printf("\n");
    
}


void Result::setLock() {
    this->locked = true;
}

void Result::setParent(Result* parent) {
    this->parent = parent;
}

void Result::setEqualType(int equalType) {
    this->equaltype = equalType;
}

Table* Result::getTable() {
    return this->table;
}
vector<Column*> Result::getColumns() {
    return this->columns;
}
Result* Result::getParent() {
    return this->parent;
}

int Result::getEqualType() {
    return this->equaltype;
}

bool Result::hasColumn(char* col_name) {
    if(this->getParent()->locked == true) return false;
    for(Column* col : columns) {
        if(strcmp(col->getName(), col_name) == 0) return true;
    }
    return false;
}


Select::Select(Database* database, vector<table_name*>* tables, vector<variable>* variables) {
}

Select::Select(Database* database, node* root, vector<table_name*>* tables, vector<variable>* variables) {

    this->database = database;
    //remove not nodes
    root = this->de_morgan(root);
    printf("Root name: %s\n", root->name);

    for(table_name* t : *tables) {
        printf("Table: %s %s\n", t->name, t->real_name);
    }
    //asign tables to the variables
    for(int i = 0, size = variables->size(); i < size; ++i) {

        variable* v = &(*variables)[i];

        if(v->table[0] == 0) {
            strcpy(v->table, database->getTableNameByVar(v->name, tables));
        } else {
            for(table_name* t : *tables) {
                if(strcmp(t->name, v->table) == 0) {
                    v->setTable(t->real_name);
                }
            }
        }
    }

    this->or_node = 0;
    this->table_count = 0;
    dfs(root);
    
    if(this->or_node > 0 && this->table_count > 1) {
        //seq scan na sve
    } else if(this->or_node > 0 && this->table_count == 1) {
        //pretraga indeksa po područjima
    } else if(this->or_node == 0) {
        //jedno područje
    }

}

void Select::dfs(node *root) {
    
    if(!root->terminal) {
        
        if(strcmp(root->name, AND_STR) == 0) { // and
            dfs(root->left);
            dfs(root->right);    
        } else { // or
            dfs(root->left);
            dfs(root->right);    
            ++or_node;
        }
    } else { //terminal root

        vector<variable> variables = *root->e1->variables;
        for(auto v : variables) {
            Table table = *database->getTable(v.table);
            tables.insert(table);
        }
    }
    
}


node* Select::de_morgan(node* root) {
    
    if(root->terminal) {
        return root;
    }
    
    if(strcmp(root->name, NOT_STR) == 0) {
        if(!root->left->terminal && strcmp(root->left->name, NOT_STR) != 0){
            node* n = new node();
            node* not1 = new node();
            node* not2 = new node();
            
            n->left = not1;
            n->right = not2;
            
            //set name
            if(strcmp(root->left->name, OR_STR) == 0) {
                memset(n->name, 0, 4);
                strcpy(n->name, AND_STR);
            } else {
                memset(n->name, 0, 4);
                strcpy(n->name, OR_STR);
            }
            strcpy(not1->name, NOT_STR);
            strcpy(not2->name, NOT_STR);
            
            //set children
            not1->left = root->left->left;
            not2->left = root->left->right;
            
            n->left = this->de_morgan(n->left);
            
            n->right = this->de_morgan(n->right);
            
            return n;
        } else if(!root->left->terminal && strcmp(root->left->name, NOT_STR) == 0) {
            //skip one not
            return de_morgan(root->left->left);
            
        } else if(root->left->terminal) {
            if(root->left->e1->equal != 0) {
                root->left->e1->equal = 3 - root->left->e1->equal;
            }
            return root->left;
        }
    } else {
        node* n1 = this->de_morgan(root->left);
        if(root->left != n1) {
            root->left = n1;
        }
        node* n2 = this->de_morgan(root->right);
        if(root->right != n2) {
            root->right = n2;
        }
    }
    return root;
}
