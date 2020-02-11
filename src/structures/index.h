#include <vector>
#include <string.h>
#include <string>
#include <stdlib.h>
using namespace std;
#define MAX_LEN 100

#ifndef INDEX_H
#define INDEX_H

enum TYPE_COL{
    ASC_ORDER, 
    DESC_ORDER
};

#define DEFAULT_INDEX_ORDER ASC_ORDER

class Index {
    public:
        void setUnique(bool unique);
        void setTable(char *table_name);
        void setName(char *name);
        void addColumn(char* col_name, TYPE_COL type);
        
        int getColNumber();
        char* getColName(int index);
        char* getTable();
        char* getName();
        TYPE_COL getColType(int index);

    private:
       vector<char*>col_names;
       vector<TYPE_COL>types;
       char table[MAX_LEN];
       char name[MAX_LEN];
       bool unique;
       size_t size;
};

#endif