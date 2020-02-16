#include "index.h"

void Index::addColumn(char* col_name, TYPE_COL type) {
    char *name = (char*) malloc(MAX_LEN);
    strcpy(name, col_name);
    this->col_names.push_back(name);
    this->types.push_back(type);
    this->size++;

    for(int i = 0, len = strlen(col_name); i < len; ++i) {
        hash = hash * P + col_name[i];
    }
}

void Index::addColumn(char* col_name) {
    this->addColumn(col_name, DEFAULT_INDEX_ORDER);
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

int Index::getHash() {
    return this->hash;
}

char* Index::getColName(int index) {
    return this->col_names[index];
}

char* Index::getTable() {
    return this->table;
}
char* Index::getName() {
    return this->name;
}

TYPE_COL Index::getColType(int index) {
    return this->types[index];
}