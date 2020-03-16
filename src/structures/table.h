#include "column.h"
#include "index.h"
#include <vector>
using namespace std;

#ifndef TABLE_H
#define TABLE_H

class Table { 
    public:
        Table();
        Table(web::json::value json);
        Table(const char* table_name, const char* database);
        
        char* getTableName();
        vector<Index*> getIndex();
        Column* getColumn(const char *name);

        bool isIndex(vector<char*> column);
        bool isColumn(char* name);

        void addIndex(Index *index);
        void addColumn(Column* column);
        
        void setTableName(char* name);
        
        void mergeTable(Table* table);

        void addRow();

        web::json::value getJSON();

    private:
        char name[MAX_LEN];
        char database[MAX_LEN];
        vector<Column*> columns;
        vector<Index*> indexs;
        int numOfRows;
};

#endif
