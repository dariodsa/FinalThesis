#include "column.h"

Column::Column(const char* name, const char* type) {
    strcpy(this->name, name);
    strcpy(this->type, type);
    this->primary_key_or_unique = false;
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