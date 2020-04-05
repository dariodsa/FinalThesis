#include "indexscan.h"

IndexScan::IndexScan(Table *table, Index* index, int len, bool retr_data) : Operation(table) {
    this->index = index;
    this->len = len;
    this->retr_data = retr_data;
}

float IndexScan::getStartCost(Database* database) {
    return 0;
}

float IndexScan::getRuntimeCost(Database* database) {
    return 0;
}

float IndexScan::getNt() {
    int s = table->getNumOfRows() / 10;
    return s;
}