#include "operation.h"

#ifndef INDEXSCAN_CLASS
#define INDEXSCAN_CLASS 1

class IndexScan : public Operation{
    public:
        IndexScan(Table *table, Index* index, int len);
    private:
        Index* index;
};

#endif