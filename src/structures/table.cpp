#include "table.h"

Table::Table() {}

Table::Table(char* table_name, char* database) {
    strcpy(this->name, table_name);
    strcpy(this->database, database);
}

char* Table::getTableName() {
    return this->name;
}


vector<Index*> Table::getIndex() {
    return this->indexs;
}

void Table::addIndex(Index *index) {
    this->indexs.push_back(index);
    return;
}

void Table::addColumn(Column *column) {
    this->columns.push_back(column);
    
    if(column->getPrimaryOrUnique()) {
        Index* i1 = new Index();
        i1->addColumn(column->getName(), DEFAULT_INDEX_ORDER);
        i1->setTable(this->name);
        i1->setName(column->getName());
        this->addIndex(i1);
    }
}

void Table::setTableName(char* name) {
    strcpy(this->name, name);
}

void Table::mergeTable(Table *table) {
    for(Column* column : table->columns) {
        this->columns.push_back(column);
    }
    for(Index* index : table->indexs) {
        this->indexs.push_back(index);
    }
    //free(table);
    return;
}