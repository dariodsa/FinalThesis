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
    for(int i = 0, len = columns1.size(); i < len; ++i) {
        string s1 = string(columns1[i]);
        string s2 = string(columns2[i]);
        this->columns_table1.push_back(s1);
        this->columns_table2.push_back(s2);
    }
}

void ForeignKey::setTable(char *table1) {
    for(int i = 0, len = strlen(table1); i < len; ++i) table1[i] = tolower(table1[i]);
    strcpy(this->table1, table1);
}

void ForeignKey::setTable2(char *table2) {
    for(int i = 0, len = strlen(table2); i < len; ++i) table2[i] = tolower(table2[i]);
    strcpy(this->table2, table2);
}

void ForeignKey::addColumnInTable1(char *column) {
    for(int i = 0, len = strlen(column); i < len; ++i) column[i] = tolower(column[i]);
    this->columns_table1.push_back(string(column));
}
void ForeignKey::addColumnInTable2(char *column) {
    for(int i = 0, len = strlen(column); i < len; ++i) column[i] = tolower(column[i]);
    this->columns_table2.push_back(string(column));
} 

char* ForeignKey::getTable2() {
    return this->table2;
}

char* ForeignKey::getTable1() {
    return this->table1;
}

bool ForeignKey::contains(vector<char*> col1, vector<char*> col2) {
    int cnt = 0;
    printf("col1 %d col2 %d\n", col1.size(), col2.size());
    for(int col_id = 0, len = col1.size(); col_id < len; ++col_id) {
        printf("%s vs %s\n", col1[col_id], col2[col_id]);
        string t1 = string(col1[col_id]);
        string t2 = string(col2[col_id]);
        int br = -1;
        int i = 0;
        for(string _s : columns_table1) {
            if(t1.compare(_s) == 0) {
                br = i;
                break;
            }
        }
        if(br == -1) continue;
        i = 0;
        if(columns_table2[br].compare(t2) == 0) ++cnt;
    }    
    
    printf("Foreign key %d %d\n", cnt, columns_table1.size());
    return cnt == columns_table1.size();
}

json::value ForeignKey::getJSON() {
    json::value value;
    value["table1"] = json::value::string(string(table1));
    value["table2"] = json::value::string(string(table2));

    int len = this->columns_table1.size();
    value["col_tables1"] = json::value::array(len);
    value["col_tables2"] = json::value::array(len);

    for(int i = 0, len = columns_table1.size(); i < len; ++i) {
        value["col_tables1"][i] = json::value::string(columns_table1[i]);
        value["col_tables2"][i] = json::value::string(columns_table2[i]);
        ++i;
    }
    
    return value;
}