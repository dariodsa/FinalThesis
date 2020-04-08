#include "foreign-key.h"
#include "table.h"
#include "column.h"

using namespace web;
using namespace std;

ForeignKey::ForeignKey() {

}

ForeignKey::ForeignKey(web::json::value json) {
    setTable((char*)json["table1"].as_string().c_str());
    setTable2((char*)json["table2"].as_string().c_str());
    web::json::array col_tables1 = json["col_tables1"].as_array();
    web::json::array col_tables2 = json["col_tables2"].as_array();
    for(auto col : col_tables1) {
        addColumnInTable1((char*)col.as_string().c_str());
    }
    for(auto col : col_tables2) {
        addColumnInTable2((char*)col.as_string().c_str());
    }
}

ForeignKey::ForeignKey(char* table1, vector<char*> columns1, char* table2, vector<char*> columns2) {
    setTable(table1);
    setTable2(table2);
    for(char* col1 : columns1) {
        this->columns_table1.push_back(col1);
    }

    for(char* col2 : columns2) {
        this->columns_table2.push_back(col2);
    }
    calculateHash2();
    calculateHash4();
}

void ForeignKey::setTable(char *table1) {
    for(int i = 0, len = strlen(table1); i < len; ++i) table1[i] = tolower(table1[i]);
    strcpy(this->table1, table1);
    calculateHash1();
}

void ForeignKey::setTable2(char *table2) {
    for(int i = 0, len = strlen(table2); i < len; ++i) table2[i] = tolower(table2[i]);
    strcpy(this->table2, table2);
    calculateHash3();
}

void ForeignKey::addColumnInTable1(char *column) {
    for(int i = 0, len = strlen(column); i < len; ++i) column[i] = tolower(column[i]);
    this->columns_table1.push_back(column);
    calculateHash2();
}
void ForeignKey::addColumnInTable2(char *column) {
    for(int i = 0, len = strlen(column); i < len; ++i) column[i] = tolower(column[i]);
    this->columns_table2.push_back(column);
    calculateHash4();
} 

char* ForeignKey::getTable2() {
    return this->table2;
}

char* ForeignKey::getTable1() {
    return this->table1;
}
    
vector<char*> ForeignKey::getColumns1() {
    return this->columns_table1;
}
vector<char*> ForeignKey::getColumns2() {
    return this->columns_table2;
}

void ForeignKey::calculateHash1() {
    h1 = 0;
    for(int i = 0, len = strlen(table1); i < len; ++i) {
        h1 = h1*P1 + table1[i];
    }
}

void ForeignKey::calculateHash2() {
    h2 = 0;
    for(char *col_name : columns_table1) {
        for(int i = 0, len = strlen(col_name); i < len; ++i) {
            h2 = h2*P2 + col_name[i];
        }
    }
}

void ForeignKey::calculateHash3() {
    h3 = 0;
    for(int i = 0, len = strlen(table2); i < len; ++i) {
        h3 = h3*P3 + table2[i];
    }
}

void ForeignKey::calculateHash4() {
    h4 = 0;
    for(char *col_name : columns_table2) {
        for(int i = 0, len = strlen(col_name); i < len; ++i) {
            h4 = h4*P4 + col_name[i];
        }
    }
}

int ForeignKey::getHash() {
    hash = h1;
    hash = hash * P + h2;
    hash = hash * P + h3;
    hash = hash * P + h4;

    return this->hash;
}
bool ForeignKey::equalTo(ForeignKey* key) {
    if(strcmp(table1, key->table1) != 0) return false;
    if(strcmp(table2, key->table2) != 0) return false;
    if(columns_table1.size() != key->columns_table1.size()) return false;
    if(columns_table2.size() != key->columns_table2.size()) return false;

    for(int i = 0, len = columns_table1.size(); i < len; ++i) {
        if(strcmp(columns_table1[i], key->columns_table1[i]) != 0) return false;
    }

    for(int i = 0, len = columns_table2.size(); i < len; ++i) {
        if(strcmp(columns_table2[i], key->columns_table2[i]) != 0) return false;
    }

    return true;
}

json::value ForeignKey::getJSON() {
    json::value value;
    value["table1"] = json::value::string(string(table1));
    value["table2"] = json::value::string(string(table2));

    value["col_tables1"] = json::value::array(columns_table1.size());
    value["col_tables2"] = json::value::array(columns_table2.size());


    for(int i = 0, len = columns_table1.size(); i < len; ++i) {
        value["col_tables1"][i] = json::value::string(string(columns_table1[i]));
    }
    for(int i = 0, len = columns_table2.size(); i < len; ++i) {
        value["col_tables2"][i] = json::value::string(string(columns_table2[i]));
    }
    
    return value;
}