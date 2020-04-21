#include "seqscan.h"

SeqScan::SeqScan(Table *table) : Operation(table) {
    printf("SEQ SCAN %s %d\n", table->getTableName(), table);
    strcpy(this->table_name, table->getTableName());
}


double SeqScan::getStartCost(Database* database) {
    return 0;
}

double SeqScan::getRuntimeCost(Database* database) {
    double block = (double)table->getSize() / (double)database->BLOCK_SIZE;
    printf("block %lf  seq page %s cost %lf\n", block , table->getTableName(), database->SEQ_PAGE_COST);
    return getNt() * database->CPU_TUPLE_COST + (block / 200) * database->SEQ_PAGE_COST;
}

double SeqScan::getNt() {
    //printf("Table: %s numOfRows: %d\n", table->getTableName(), table->getNumOfRows());
    return this->table->getNumOfRows();
}
