#include "operation.h"
#include "../../db/program.h"

#ifndef HASH_JOIN_H
#define HASH_JOIN_H

class HashJoin : private Operation{
    public:
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
    
};

#endif