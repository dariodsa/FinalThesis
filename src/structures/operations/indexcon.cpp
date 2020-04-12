#include "indexcon.h"

IndexCon::IndexCon(Table *table, Index* index, int len, bool retr_data) : Operation(table) {
    this->len = len;
    this->index = index;
    this->retr_data = retr_data;
    printf("INDEX CON %s\n", table->getTableName());
}

double IndexCon::getStartCost(Database* database) {
    int n = database->getTable(index->getTable())->getNumOfRows();
    int height = log((n+1) / 2);
    return height * (database->CPU_INDEX_TUPLE_COST + database->CPU_OPERATOR_COST);
}

double IndexCon::getRuntimeCost(Database* database) {
    double nt = getNt();
    double cost = nt * database->CPU_INDEX_TUPLE_COST;
    
    if(retr_data) cost += nt * database->RANDOM_PAGE_COST;
    return cost;
}

double IndexCon::getNt() {
    double ratio = 1.0 / (index->getColNumber() - len + 1);
    
    if(index->getUnique()) {
        return 1 + 1 * ((index->getColNumber() - len));
    } else {
        return 3 + 6 * (1 - ratio);
    }
}