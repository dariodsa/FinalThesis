#include "indexcon.h"

IndexCon::IndexCon(Table *table, Index* index, int len) : Operation(table) {
    this->len = len;
    this->index = index;
}

float IndexCon::getStartCost(Database* database) {
    return 0;
}

float IndexCon::getRuntimeCost(Database* database) {
    return getNt() * database->CPU_INDEX_TUPLE_COST;
}

float IndexCon::getNt() {
    if(index->getUnique()) {
        return 1;
    } else {
        return 3;
    }
}