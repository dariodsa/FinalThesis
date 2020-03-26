#ifndef MAX_LEN
#define MAX_LEN 50
#endif

#ifndef INDEX_H
#define INDEX_H

#include "database.h"
#include <vector>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <cpprest/json.h>

using namespace std;



enum TYPE_COL{
    ASC_ORDER = 1
    , DESC_ORDER = 2
};

#define DEFAULT_INDEX_ORDER ASC_ORDER

const int P = 103;

class Database;

class Index {
    public:
        Index();
        Index(web::json::value json);

        void setUnique(bool unique);
        void setTable(const char *table_name);
        void setName(const char *name);
        
        void addColumn(const char* col_name);
        void addColumn(const char* col_name, TYPE_COL type);
        
        int getColNumber();
        int getHash();
        char* getColName(int index);
        bool getUnique();
        char* getTable();
        char* getName();
        TYPE_COL getColType(int index);
        float getCost(Database* database);
        web::json::value getJSON();

    private:
       vector<char*>col_names;
       vector<TYPE_COL>types;
       
       int hash = 0;
       
       char table[MAX_LEN];
       char name[MAX_LEN];

       bool unique;
};

#endif