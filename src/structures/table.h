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
        vector<Index*> getIndex();

        bool isIndex(vector<char*> column);
        
        void addIndex(Index *index);
        void addColumn(Column* column);
        
        void setTableName(char* name);
        
        void mergeTable(Table* table);

        void addRow();
    private:
        char name[MAX_LEN];
        char database[MAX_LEN];
        vector<Column*> columns;
        vector<Index*> indexs;
        int numOfRows;
};

#endif
