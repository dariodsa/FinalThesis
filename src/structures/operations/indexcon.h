#include "operation.h"

class IndexCon : public Operation{
    public:
        IndexCon(Table *table, Index* index);
};