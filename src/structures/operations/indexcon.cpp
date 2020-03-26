#include "indexcon.h"

IndexCon::IndexCon(Table *table, Index* index, int len) : Operation(table) {
    this->index = index;
}