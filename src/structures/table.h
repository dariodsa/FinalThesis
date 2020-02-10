#include "column.h"
#include "index.h"
#include <vector>
using namespace std;

#ifndef TABLE_H
#define TABLE_H

class Table { 
    public:
        
        char* getTableName();
        bool isIndex(char* column, ...);
    private:
        char name[MAX_LEN];
        char database[MAX_LEN];
        vector<Column*> columns;
        vector<Index*> indexs;
};

#endif
