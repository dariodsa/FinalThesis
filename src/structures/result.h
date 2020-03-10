#include <string.h>
#include <vector>

#ifndef RESULT_H
#define RESULT_H 

extern char AND_STR[4];
extern char NOT_STR[4];
extern char OR_STR[3];

struct variable {

    int depth;
    char name[100];
    bool t;
    char table[100];
    variable() {}
    variable(const char* _name, int _depth) {
        depth = depth;
        strcpy(name, _name);
        t = false;
    }
    variable(const char* _name, char* _table, int _depth) {
        depth = depth;
        strcpy(name, _name);
        t = true;
        strcpy(table, _table);
    }
};

struct table_name {

    int depth;
    char name[100];
    char real_name[100];
    table_name(const char* _name, const char* _real_name, int _depth) {
        depth = _depth;
        strcpy(name, _name);
        strcpy(real_name, _real_name);
    }
};

struct select_state {
    std::vector<table_name*>* tables;
    std::vector<variable>* select_variable;
    bool SELECT_LIST = false;
    select_state() {
        SELECT_LIST = true;
        select_variable = new std::vector<variable>();
    }
};

struct expression_info {
    int equal = 0;
    std::vector<variable>* variables;
    expression_info() {
        variables = new std::vector<variable>();
    }
};

struct node {
    bool terminal = false;
    char name[5];
    expression_info* e1;
    node *left = 0; 
    node *right = 0;
    node() {
        terminal = false;
    }
};



enum TYPE{
      SELECT_VARIABLE
    , CONDITION_VARIABLE
    , SORT_VARIABLE
    , GROUP_VARIABLE 
};


class Sorting{
    public:
        Sorting(int num_rows, bool disc);
    private:
        int num_rows;
        bool disc;
};

class Select{
    public: 
        Select(node *node);
    private:
        std::vector<expression_info> bfs_populate(node *node);
        
        node* de_morgan(node* root);

};




#endif