#include "table.h"

Table::Table(char* table_name, char* database) {
    strcpy(this->name, table_name);
    strcpy(this->database, database);
}

char* Table::getTableName() {
    return this->name;
}

void Table::addIndex(Index *index) {
    this->indexs.push_back(index);
    return;
}
vector<Index*> Table::getIndex() {
    return this->indexs;
}