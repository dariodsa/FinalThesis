#include <vector>
#include <cpprest/json.h>

#ifndef MAX_LEN
#define MAX_LEN 50
#endif

#ifndef FOREIGN_KEY_H
#define FOREIGN_KEY_H

class Table;
class Column;

class ForeignKey{
    public:
    ForeignKey();
    ForeignKey(web::json::value);
    ForeignKey(char* table, std::vector<char*> columns_table1, char* table2, std::vector<char*> columns_table2);
    void setTable(char *table);
    void addColumnInTable1(char* column);

    void setTable2(char *table2);
    void addColumnInTable2(char* column);

    char* getTable1();
    char* getTable2();

    bool isIndex() { return false; }

    bool contains(std::vector<char*> col1, std::vector<char*> col2);

    web::json::value getJSON();

    private:

    char table1[MAX_LEN];
    char table2[MAX_LEN];
    std::vector<std::string>columns_table1;
    std::vector<std::string>columns_table2;
    
    int hash;

    int h1, h2, h3, h4;

    const int P1 = 227;
    const int P2 = 229; 
    const int P3 = 233;
    const int P4 = 239;
    const int P = 241;
};

#endif