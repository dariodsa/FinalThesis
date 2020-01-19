#include "column.h"
#include "index.h"
#include <vector>
using namespace std;

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
