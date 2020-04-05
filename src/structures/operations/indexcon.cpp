#include "indexcon.h"

IndexCon::IndexCon(Table *table, Index* index, int len, bool retr_data) : Operation(table) {
    this->len = len;
    this->index = index;
    this->retr_data = retr_data;
}

float IndexCon::getStartCost(Database* database) {
    return 0;
}

float IndexCon::getRuntimeCost(Database* database) {
    return 0;
}

float IndexCon::getNt() {
    if(index->getUnique()) {
        return 1;
    } else {
        return 3;
    }
}