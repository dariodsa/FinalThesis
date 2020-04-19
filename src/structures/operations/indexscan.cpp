#include "indexscan.h"

IndexScan::IndexScan(Table *table, Index* index, int len, bool retr_data) : Operation(table) {
    this->index = index;
    this->len = len;
    this->retr_data = retr_data;
    printf("INDEX SCAN %s %s\n", index, table->getTableName());
    index->getColNumber();
}

double IndexScan::getStartCost(Database* database) {
    int n = table->getNumOfRows();
    int height = log((n+1) / 2);
    return height * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
}

double IndexScan::getRuntimeCost(Database* database) {
    double nt = getNt();
    double cost = nt * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
    if(retr_data) cost += nt * database->RANDOM_PAGE_COST;
    return cost;    
}

bool IndexScan::getRetrData() {
    return this->retr_data;
}

double IndexScan::getNt() {
    double rate = 1.0 / (float) (index->getColNumber() - len + 1);
    int s = ( table->getNumOfRows() * 0.1 * (1.0 + rate));
    return s;
}

Index* IndexScan::getIndex() {
    return this->index;
}