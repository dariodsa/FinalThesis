#include <vector>

#ifndef FOREIGN_KEY_H
#define FOREIGN_KEY_H

class Table;
class Column;

class ForeignKey{
    public:
    ForeignKey();
    ForeignKey(const char* table, std::vector<char*> columns_table1, const char* table2, std::vector<char*> columns_table2);
    void setTable(const char *table);
    void addColumnInTable1(char* column);

    void setTable2(const char *table2);
    void addColumnInTable2(char* column);

    private:
    const char* table1;
    const char* table2;
    std::vector<char*> columns_table1;
    std::vector<char*> columns_table2;
};

#endif