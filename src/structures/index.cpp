#include "index.h"

void Index::addColumn(char* col_name, TYPE_COL type) {
    char *name = (char*) malloc(MAX_LEN);
    strcpy(name, col_name);
    this->col_names.push_back(name);
    this->types.push_back(type);
    this->size++;
}

void Index::setUnique(bool unique) {
    this->unique = unique;
}

void Index::setTable(char *table_name) {
    strcpy(this->table, table_name);
}

void Index::setName(char *name) {
    strcpy(this->name, name);
}

int Index::getColNumber() {
    return this->size;
}

char* Index::getColName(int index) {
    return this->col_names[index];
}
TYPE_COL Index::getColType(int index) {
    return this->types[index];
}