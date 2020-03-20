#include "operation.h"

class IndexScan : public Operation{
    public:
        IndexScan(Table *table, Index* index);
};