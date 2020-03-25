#include "operation.h"

#ifndef INDEXCON_CLASS
#define INDEXCON_CLASS 1

class IndexCon : public Operation{
    public:
        IndexCon(Table *table, Index* index);
};

#endif