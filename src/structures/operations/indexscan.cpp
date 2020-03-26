#include "indexscan.h"

IndexScan::IndexScan(Table *table, Index* index, int len) : Operation(table) {
    this->index = index;
}