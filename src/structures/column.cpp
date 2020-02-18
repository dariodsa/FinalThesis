#include "column.h"
#include <string>

using namespace std;
using namespace web;

Column::Column(const char* name, const char* type) {
    strcpy(this->name, name);
    strcpy(this->type, type);
    
}

Column::Column(web::json::value json) : Column(
                                                json["name"].as_string().c_str()
                                              , json["type"].as_string().c_str()) {
    
    this->setPrimaryOrUnique(json["primary_key_or_unique"].as_bool());
}

char* Column::getName() {
    return this->name;
}

char* Column::getType() {
    return this->type;
}

bool Column::getPrimaryOrUnique() {
    return this->primary_key_or_unique;
}

void Column::setPrimaryOrUnique(bool value) {
    this->primary_key_or_unique = value;
}

web::json::value Column::getJSON() {
    web::json::value value;
    value["name"] = json::value::string(string(name));
    value["type"] = json::value::string(string(type));
    value["primary_key_or_unique"] = json::value::boolean(primary_key_or_unique);

    return value;
}