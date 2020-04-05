#include "operation.h"
#include "../../db/program.h"

#ifndef MERGE_JOIN_H
#define MERGE_JOIN_H

class MergeJoin : public Operation{
    public:
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
    private:
    
};

#endif