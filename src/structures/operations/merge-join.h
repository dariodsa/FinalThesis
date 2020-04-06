#include "operation.h"
#include "../../db/program.h"

#ifndef MERGE_JOIN_H
#define MERGE_JOIN_H

class MergeJoin : public Operation{
    public:
        MergeJoin(bool foreign_key = false);
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
        float sorting_cost = 0;
        int nt = 0;
    
};

#endif