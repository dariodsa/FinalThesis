#include "indexscan.h"

IndexScan::IndexScan(Table *table, Index* index) : Operation(table) {
    this->index = index;
}