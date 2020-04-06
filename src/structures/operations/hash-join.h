#include "operation.h"
#include "../../db/program.h"

#ifndef HASH_JOIN_H
#define HASH_JOIN_H

class HashJoin : public Operation{
    public:
        HashJoin(bool foreign_key);
        virtual float getStartCost(Database* database);
        virtual float getRuntimeCost(Database* database);
        virtual float getNt();
    private:
        int nt;
};

#endif