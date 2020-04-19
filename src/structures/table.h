#ifndef TABLE_H
#define TABLE_H

class Index;
class Database;

#include <vector>
#include "column.h"
#include "index.h"
#include "database.h"
using namespace std;



class Table { 
    public:
        Table();
        Table(web::json::value json);
        Table(const char* table_name, const char* database);
        
        char* getTableName();
        vector<Index*> getIndex();
        Column* getColumn(const char *name);
        float getCost(Database* database, bool full_table);
        web::json::value getJSON();
        long long getSize();
        int getNumOfRows();

        bool isIndex(vector<char*> column);
        Index* getIndexByCol(vector<char*> columns);
        
        bool isColumn(char* name);

        void addIndex(Index *index);
        void addColumn(Column* column);
        
        void setTableName(char* name);
        
        void mergeTable(Table* table);

        void addRow();

        bool operator<(const Table &t1) const {
            if(t1.numOfRows == numOfRows) {
                return strcmp(name, t1.name) < 0;
            } else {
                return numOfRows > t1.numOfRows;
            }
        }
    private:
        char name[MAX_LEN];
        char database[MAX_LEN];
        vector<Column*> columns;
        vector<Index*> indexs;
        int numOfRows;
};

#endif
