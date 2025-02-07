#include "table.h"
#include "../db/program.h"

using namespace web;

Table::Table() {
    memset(this->name, 0, MAX_LEN * sizeof(char));
    memset(this->database, 0, MAX_LEN * sizeof(char));
}

Table::Table(web::json::value json) : Table(json["table_name"].as_string().c_str(), json["database"].as_string().c_str()){
    web::json::array indexs = json["indexs"].as_array();
    for(web::json::value index : indexs) {
        Index* i1 = new Index(index);
        addIndex(i1);
    }

    web::json::array columns = json["columns"].as_array();
    for(web::json::value column : columns) {
        Column* c1 = new Column(column);
        this->columns.push_back(c1);
    }

    this->numOfRows = json["numOfRows"].as_integer();
}

Table::Table(const char* table_name, const char* database) {
    strcpy(this->name, table_name);
    strcpy(this->database, database);
}


web::json::value Table::getJSON() {
    json::value json;

    json["table_name"] = json::value::string(string(name));
    json["database"] = json::value::string(string(database));

    json["indexs"] = json::value::array(indexs.size());
    json["columns"] = json::value::array(columns.size());

    for(int i = 0, len = columns.size(); i < len; ++i) {
        json["columns"][i] = columns[i]->getJSON();
    }
    for(int i = 0, len = indexs.size(); i < len; ++i) {
        json["indexs"][i] = indexs[i]->getJSON();
    }

    json["numOfRows"] = json::value::number(numOfRows);

    return json;
}


char* Table::getTableName() {
    return this->name;
}

int Table::getNumOfRows() {
    return this->numOfRows;
}

vector<Index*> Table::getIndex() {
    return this->indexs;
}

Column* Table::getColumn(const char *name) {
    for(Column* col : columns) {
        if(strcmp(name, col->getName()) == 0) return col;
    }
    return NULL;
}

float Table::getCost(Database* database, bool full_table) {
    float cost = 0;
    
    
    //database->addTable(this);
    
    return cost;
}


long long Table::getSize() {
    return (long long) this->numOfRows * (long long)  this->columns.size() * (long long) Program::AVERAGE_DATA_SIZE;
}

void Table::addIndex(Index *index) {
    this->indexs.push_back(index);
    index->setTable(name);
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

Index* Table::getIndexByCol(vector<char*> columns) {
    for(Index *index : indexs) {
        if(columns.size() == index->getColNumber()) {
            int it = 0;
            for(char* col : columns) {
                if(strcmp(col, index->getColName(it)) != 0) break;
                ++it;
            }
            if(it == index->getColNumber()) return index;
        }
    }
    return NULL;
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

bool Table::isColumn(char* name) {
    for(auto column: columns) {
        if(strcmp(column->getName(), name) == 0) return true;
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
