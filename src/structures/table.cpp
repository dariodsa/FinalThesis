#include "table.h"

using namespace web;

Table::Table() {}

Table::Table(web::json::value json) : Table(json["table_name"].as_string().c_str(), json["database"].as_string().c_str()){
    web::json::array indexs = json["indexs"].as_array();
    for(web::json::value index : indexs) {
        Index* i1 = new Index(index);
        this->indexs.push_back(i1);
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
