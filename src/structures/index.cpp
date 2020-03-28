#include "index.h"

using namespace web;

Index::Index() {
    memset(this->name, 0, MAX_LEN * sizeof(char));
    this->unique = false;
}

Index::Index(web::json::value json) : Index(){
    this->setName(json["name"].as_string().c_str());
    this->setTable(json["table"].as_string().c_str());
    
    this->setUnique(json["unique"].as_bool());
    this->hash = json["hash"].as_integer();

    web::json::array col_names = json["col_names"].as_array();
    web::json::array col_types = json["col_types"].as_array();
    int it = 0;
    for(auto col_name : col_names) {
        int col_type = col_types[it].as_integer();
        if(col_type == 1) {
            addColumn(col_name.as_string().c_str(), ASC_ORDER);
        }
        else { 
            addColumn(col_name.as_string().c_str(), DESC_ORDER);
        }
        ++it;
    }

    
    
}

web::json::value Index::getJSON() {
    web::json::value value;
    value["name"] = json::value::string(string(name));
    value["table"] = json::value::string(string(table));

    value["unique"] = json::value::boolean(unique);
    value["hash"] = json::value::number(hash);

    value["col_names"] = json::value::array(col_names.size());
    value["col_types"] = json::value::array(col_names.size());


    for(int i = 0, len = col_names.size(); i < len; ++i) {
        value["col_names"][i] = json::value::string(string(col_names[i]));

        if(types[i] == ASC_ORDER) {
            value["col_types"][i] = json::value::number(1);
        } else {
            value["col_types"][i] = json::value::number(2);
        }
    }

    return value;
}

bool Index::getUnique() {
    return this->unique;
}

void Index::addColumn(const char* col_name, TYPE_COL type) {
    char *name = (char*) malloc(MAX_LEN);
    strcpy(name, col_name);
    this->col_names.push_back(name);
    this->types.push_back(type);

    for(int i = 0, len = strlen(col_name); i < len; ++i) {
        hash = hash * P + col_name[i];
    }
}

void Index::addColumn(const char* col_name) {
    this->addColumn(col_name, DEFAULT_INDEX_ORDER);
}

void Index::setUnique(bool unique) {
    this->unique = unique;
}

void Index::setTable(const char *table_name) {
    strcpy(this->table, table_name);
}

void Index::setName(const char *name) {
    strcpy(this->name, name);
}

int Index::getColNumber() {
    return this->col_names.size();
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

float Index::getCost(Database* database) {
    float cost = 0;
    if(database->isIndexLoaded(this)) {
        //index is loaded
        //TODO
    } else {
        //TODO
        //index is not loaded
    }
    return cost;
}