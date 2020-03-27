#include "indexcon.h"

IndexCon::IndexCon(Table *table, Index* index, int len, bool retr_data) : Operation(table) {
    this->len = len;
    this->index = index;
    this->retr_data = retr_data;
}

