#include "column.h"

Column::Column(const char* name, const char* type) {
    strcpy(this->name, name);
    strcpy(this->type, type);
}

char* Column::getName() {
    return this->name;
}

char* Column::getType() {
    return this->type;
}