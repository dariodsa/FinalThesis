#include "indexcon.h"

IndexCon::IndexCon(Table *table, Index* index, int len, bool retr_data) : Operation(table) {
    this->len = len;
    this->index = index;
    this->retr_data = retr_data;
}

float IndexCon::getStartCost(Database* database) {
    int n = database->getTable(index->getTable())->getNumOfRows();
    int height = log((n+1) / 2);
    return height * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
}

float IndexCon::getRuntimeCost(Database* database) {
    int nt = getNt();
    float cost = nt * database->CPU_INDEX_TUPLE_COST;
    if(retr_data) cost += nt * database->RANDOM_PAGE_COST;
    return cost;
}

float IndexCon::getNt() {
    float ratio = 1.0 / (index->getColNumber() - len);
    if(index->getUnique()) {
        return 1 + 1 * ((index->getColNumber() - len));
    } else {
        return 3 + 6 * (1 - ratio);
    }
}