#include "operation.h"
#include "indexscan.h"

class RetrData : public Operation{
    public:
        RetrData(IndexScan* index);
};