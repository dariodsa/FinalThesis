#include "operation.h"
#include "indexscan.h"

#ifndef RETRDATA_CLASS
#define RETRDATA_CLASS 1

class RetrData : public Operation{
    public:
        RetrData(IndexScan* index);
};

#endif