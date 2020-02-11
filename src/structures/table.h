#include "column.h"
#include "index.h"
#include <vector>
using namespace std;

#ifndef TABLE_H
#define TABLE_H

class Table { 
    public:
        Table();
        Table(char* table_name, char* database);
        char* getTableName();
        bool isIndex(char* column, ...);
        void addIndex(Index *index);
        void addColumn(Column* column);
        void setTableName(char* name);
        vector<Index*> getIndex();
    private:
        char name[MAX_LEN];
        char database[MAX_LEN];
        vector<Column*> columns;
        vector<Index*> indexs;
};

#endif
