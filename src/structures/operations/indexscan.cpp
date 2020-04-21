#include "indexscan.h"

IndexScan::IndexScan(Table *table, Index* index, int len, bool retr_data, bool full) : Operation(table) {
    this->index = index;
    this->len = len;
    this->retr_data = retr_data;
    this->full = full;
    printf("INDEX SCAN %d %s %d\n", index, table->getTableName(), retr_data);
    index->getHash();
}

double IndexScan::getStartCost(Database* database) {
    int n = table->getNumOfRows();
    int height = log((n+1) / 2);
    return height * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
}

double IndexScan::getRuntimeCost(Database* database) {
    double nt = getNt();
    double cost = 0;
    if(full) cost = nt / 4 * database->CPU_INDEX_TUPLE_COST;
    else cost = nt * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
    if(retr_data) cost += nt / database->BLOCK_SIZE * database->SEQ_PAGE_COST;
    return cost;    
}

bool IndexScan::getRetrData() {
    return this->retr_data;
}

double IndexScan::getNt() {
    if(full) return table->getNumOfRows();
    double rate = 1.0 / (float) (index->getColNumber() - len + 1);
    double s = ( (double) table->getNumOfRows() * 0.05 * (1.0 + rate));
    return s;
}

Index* IndexScan::getIndex() {
    return this->index;
}