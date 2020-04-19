#include "seqscan.h"

SeqScan::SeqScan(Table *table) : Operation(table) {
    printf("SEQ SCAN %s %d\n", table->getTableName(), table);
    strcpy(this->table_name, table->getTableName());
}

Table* SeqScan::getTable() {
    return this->table;
}

double SeqScan::getStartCost(Database* database) {
    return 0;
}

double SeqScan::getRuntimeCost(Database* database) {
    return getNt() * database->CPU_TUPLE_COST;
}

double SeqScan::getNt() {
    //printf("Table: %s numOfRows: %d\n", table->getTableName(), table->getNumOfRows());
    return this->table->getNumOfRows();
}
