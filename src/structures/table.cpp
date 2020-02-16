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

void Table::addRow() {
    this->numOfRows++;
}

bool Table::isIndex(vector<char*> column) {
    int _hash = 0;
    for(int i = 0, len = column.size(); i < len; ++i) {
        for(int j = 0, column_len = strlen(column[i]); j < column_len; ++j) {
            _hash = _hash * P + column[i][j];        
        }
    }
    
    for(int i = 0, len = indexs.size(); i < len; ++i) {
        if(indexs[i]->getHash() == _hash && indexs[i]->getColNumber() == column.size()) {
            //additional check
            bool same = true;
            for(int col_id = 0, num = indexs[i]->getColNumber(); col_id < num; ++col_id) {
                if(strcmp(indexs[i]->getColName(col_id), column[col_id]) != 0) {
                    same = false;
                    break;
                }
            }
            if(same) return true;
        }
    }
    return false;
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