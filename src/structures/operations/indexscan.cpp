#include "indexscan.h"

IndexScan::IndexScan(Table *table, Index* index, int len, bool retr_data) : Operation(table) {
    this->index = index;
    this->len = len;
    this->retr_data = retr_data;
}

float IndexScan::getStartCost(Database* database) {
    int n = database->getTable(index->getTable())->getNumOfRows();
    int height = log((n+1) / 2);
    return height * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
}

float IndexScan::getRuntimeCost(Database* database) {
    int nt = getNt();
    float cost = nt * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
    if(retr_data) cost += nt * database->RANDOM_PAGE_COST;
    return cost;    
}

float IndexScan::getNt() {
    float rate = 1.0 / (float) (index->getColNumber() - len);
    int s = ( table->getNumOfRows() * 0.1 * (1.0 + rate));
    return s;
}