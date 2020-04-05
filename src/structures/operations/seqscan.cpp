#include "seqscan.h"

SeqScan::SeqScan(Table *table) : Operation(table) {
}

Table* SeqScan::getTable() {
    return this->table;
}

float SeqScan::getStartCost(Database* database) {
    return 0;
}

float SeqScan::getRuntimeCost(Database* database) {
    return getNt() * database->CPU_TUPLE_COST;
}

float SeqScan::getNt() {
    return this->table->getNumOfRows();
}
