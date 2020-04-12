#include "operation.h"
#include "../../db/program.h"

#ifndef HASH_JOIN_H
#define HASH_JOIN_H

class HashJoin : public Operation{
    public:
        HashJoin(bool foreign_key);
        virtual double getStartCost(Database* database);
        virtual double getRuntimeCost(Database* database);
        virtual double getNt();
    private:
        int nt;
};

#endif