#include "indexscan.h"

IndexScan::IndexScan(Table *table, Index* index, int len) : Operation(table) {
    this->index = index;
    this->len = len;
}

float IndexScan::getStartCost(Database* database) {
    return 0;
}

float IndexScan::getRuntimeCost(Database* database) {
    int nt = getNt();
    float cost = nt * database->CPU_INDEX_TUPLE_COST;
    return cost;    
}

float IndexScan::getNt() {
    int s = table->getNumOfRows() / 10;
    return s;
}