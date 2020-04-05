#include "seqscan.h"

SeqScan::SeqScan(Table *table) : Operation(table) {
}

float SeqScan::getStartCost(Database* database) {
    return 0;
}

float SeqScan::getRuntimeCost(Database* database) {
    return 0;
}

float SeqScan::getNt() {
    return this->table->getNumOfRows();
}
